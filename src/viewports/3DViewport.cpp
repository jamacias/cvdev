#include "3DViewport.h"

#include "Corrade/Utility/Debug.h"
#include "Magnum/GL/AbstractFramebuffer.h"
#include "Magnum/GL/Framebuffer.h"
#include "Magnum/ImGuiIntegration/Widgets.h"
#include "Magnum/Magnum.h"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Image.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Trade/MeshData.h>
#include <imgui.h>

ThreeDViewport::ThreeDViewport(const Range2Di& viewportArea, const std::shared_ptr<Scene3D> scene)
: viewportArea_(viewportArea)
, scene_(scene)
{
    using namespace Math::Literals;
    camera_ = std::make_unique<Camera>(*scene_);
    camera_->rotateZLocal(45.0_degf).rotateXLocal(70.0_degf).translateLocal(Vector3::zAxis(4.0f));
    camera_->setProjectionMatrix(
        Matrix4::perspectiveProjection(45.0_degf, Vector2{viewportArea_.size()}.aspectRatio(), 0.01f, 100.0f));

    lastDepth_ = ((camera_->projectionMatrix() * camera_->cameraMatrix()).transformPoint({}).z() + 1.0f) * 0.5f;

    framebuffer_  = GL::Framebuffer{viewportArea_};
    colorTexture_ = GL::Texture2D{};
    colorTexture_.setStorage(1, GL::TextureFormat::RGBA8, viewportArea_.size());
    depthBuffer_ = GL::Renderbuffer{};
    depthBuffer_.setStorage(GL::RenderbufferFormat::DepthComponent24, viewportArea_.size());
    framebuffer_.attachTexture(GL::Framebuffer::ColorAttachment{0}, colorTexture_, 0)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, depthBuffer_)
        .mapForDraw({{0, GL::Framebuffer::ColorAttachment{0}}});

    // Setup the borders of the viewport
    mesh_ = MeshTools::compile(Primitives::squareWireframe());
}

Float ThreeDViewport::depthAt(const Vector2& windowPosition)
{
    /* First scale the position from being relative to window size to being
       relative to framebuffer size as those two can be different on HiDPI
       systems */
    // const Vector2i position =
    //     windowPosition * applicationContext_.framebufferSize() / Vector2{applicationContext_.windowSize()};
    const Vector2i position = Vector2i{windowPosition};
    const Vector2i fbPosition{position.x(), framebuffer_.viewport().sizeY() - position.y() - 1};

    Image2D data = framebuffer_.read(Range2Di::fromSize(fbPosition, Vector2i{1}).padded(Vector2i{2}),
                                     {GL::PixelFormat::DepthComponent, GL::PixelType::Float});

    /* TODO: change to just Math::min<Float>(data.pixels<Float>() when the
       batch functions in Math can handle 2D views */
    return Math::min<Float>(data.pixels<Float>().asContiguous());
}

Vector3 ThreeDViewport::unproject(const Vector2& windowPosition, Float depth) const
{
    /* We have to take window size, not framebuffer size, since the position is
       in window coordinates and the two can be different on HiDPI systems */
    const Vector2i viewSize = viewportArea_.size();
    const Vector2  viewPosition{windowPosition.x(), viewSize.y() - windowPosition.y() - 1};
    const Vector3  in{2.0f * viewPosition / Vector2{viewSize} - Vector2{1.0f}, depth * 2.0f - 1.0f};

    return camera_->projectionMatrix().inverted().transformPoint(in);
}

void ThreeDViewport::handlePointerPressEvent(Platform::Application::PointerEvent& event)
{
    using Pointer = Platform::Application::Pointer;

    if (!viewportArea_.contains(Vector2i{event.position()}))
        return;

    if (!event.isPrimary() || !(event.pointer() & (Pointer::MouseLeft)))
        return;

    /* Update the move position on press as well so touch movement (that emits
       no hover pointerMoveEvent()) works without jumps */
    lastPosition_ = event.position();

    const Float currentDepth = depthAt(event.position());
    const Float depth        = currentDepth == 1.0f ? lastDepth_ : currentDepth;
    translationPoint_        = unproject(event.position(), depth);
    /* Update the rotation point only if we're not zooming against infinite
       depth or if the original rotation point is not yet initialized */
    if (currentDepth != 1.0f || rotationPoint_.isZero())
    {
        rotationPoint_ = translationPoint_;
        lastDepth_     = depth;
    }
}

void ThreeDViewport::handlePointerReleaseEvent([[maybe_unused]] Platform::Application::PointerEvent& event) {}

void ThreeDViewport::handlePointerMoveEvent(Platform::Application::PointerMoveEvent& event)
{
    using Pointer  = Platform::Application::Pointer;
    using Modifier = Platform::Application::Modifier;
    using namespace Math::Literals;

    if (!event.isPrimary() || !(event.pointers() & (Pointer::MouseLeft)))
        return;

    if (Math::isNan(lastPosition_).all())
        lastPosition_ = event.position();
    const Vector2 delta = event.position() - lastPosition_;
    lastPosition_       = event.position();

    /* Translate */
    if (event.modifiers() & Modifier::Shift)
    {
        const Vector3 p = unproject(event.position(), lastDepth_);
        camera_->translateLocal(translationPoint_ - p); /* is Z always 0? */
        translationPoint_ = p;

        /* Rotate around rotation point */
    }
    else
        camera_->transformLocal(Matrix4::translation(rotationPoint_) *
                                Matrix4::rotationX(-0.01_radf * delta.y()) *
                                Matrix4::rotationY(-0.01_radf * delta.x()) *
                                Matrix4::translation(-rotationPoint_));
}

void ThreeDViewport::handleScrollEvent(Platform::Application::ScrollEvent& event)
{
    if (!viewportArea_.contains(Vector2i{event.position()}))
        return;

    const Float   currentDepth = depthAt(event.position());
    const Float   depth        = currentDepth == 1.0f ? lastDepth_ : currentDepth;
    const Vector3 p            = unproject(event.position(), depth);
    /* Update the rotation point only if we're not zooming against infinite
       depth or if the original rotation point is not yet initialized */
    if (currentDepth != 1.0f || rotationPoint_.isZero())
    {
        rotationPoint_ = p;
        lastDepth_     = depth;
    }

    const Float direction = event.offset().y();
    if (!direction)
        return;

    /* Move towards/backwards the rotation point in cam coords */
    camera_->translateLocal(rotationPoint_ * direction * 0.1f);

    event.setAccepted();
}

void ThreeDViewport::draw(SceneGraph::DrawableGroup3D& drawables)
{
    using namespace Math::Literals;

    framebuffer_.bind();
    framebuffer_.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    camera_->draw(drawables);
    shader_.setColor(Color3::fromHsv({35.0_degf, 1.0f, 1.0f})).draw(mesh_);

    GL::defaultFramebuffer.bind();

    ImGui::Begin("3D Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGuiIntegration::image(colorTexture_, Vector2{viewportArea_.size()});
    const auto& windowPos  = ImGui::GetWindowPos();
    const auto& windowSize = ImGui::GetWindowSize();
    viewportArea_          = {{static_cast<Int>(windowPos.x), static_cast<Int>(windowPos.y)},
                              {static_cast<Int>(windowSize.x), static_cast<Int>(windowSize.y)}};
    ImGui::End();
}
