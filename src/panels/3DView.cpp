#include "3DView.h"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/Image.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Trade/MeshData.h>

ThreeDView::ThreeDView(const Platform::Application& applicationContext, const std::shared_ptr<Scene3D> scene)
: applicationContext_(applicationContext)
, scene_(scene)
{
    using namespace Math::Literals;
    camera_ = std::make_unique<Camera>(*scene_);
    camera_->rotateZLocal(45.0_degf).rotateXLocal(70.0_degf).translateLocal(Vector3::zAxis(4.0f));
    camera_->setProjectionMatrix(Matrix4::perspectiveProjection(
        45.0_degf, Vector2{applicationContext_.windowSize()}.aspectRatio(), 0.01f, 100.0f));

    lastDepth_ = ((camera_->projectionMatrix() * camera_->cameraMatrix()).transformPoint({}).z() + 1.0f) * 0.5f;
    setRelativeViewport({Vector2{0.0f, 0.0f}, Vector2{1.0f, 1.0f}});

    // Setup the borders of the viewport
    mesh_ = MeshTools::compile(Primitives::squareWireframe());
}

Float ThreeDView::depthAt(const Vector2& windowPosition) const
{
    /* First scale the position from being relative to window size to being
       relative to framebuffer size as those two can be different on HiDPI
       systems */
    const Vector2i position
        = windowPosition * applicationContext_.framebufferSize() / Vector2{applicationContext_.windowSize()};
    const Vector2i fbPosition{position.x(), GL::defaultFramebuffer.viewport().sizeY() - position.y() - 1};

    GL::defaultFramebuffer.mapForRead(GL::DefaultFramebuffer::ReadAttachment::Front);
    Image2D data = GL::defaultFramebuffer.read(Range2Di::fromSize(fbPosition, Vector2i{1}).padded(Vector2i{2}),
                                               {GL::PixelFormat::DepthComponent, GL::PixelType::Float});

    /* TODO: change to just Math::min<Float>(data.pixels<Float>() when the
       batch functions in Math can handle 2D views */
    return Math::min<Float>(data.pixels<Float>().asContiguous());
}

Vector3 ThreeDView::unproject(const Vector2& windowPosition, Float depth) const
{
    /* We have to take window size, not framebuffer size, since the position is
       in window coordinates and the two can be different on HiDPI systems */
    const Vector2i viewSize = applicationContext_.windowSize();
    const Vector2  viewPosition{windowPosition.x(), viewSize.y() - windowPosition.y() - 1};
    const Vector3  in{2.0f * viewPosition / Vector2{viewSize} - Vector2{1.0f}, depth * 2.0f - 1.0f};

    return camera_->projectionMatrix().inverted().transformPoint(in);
}

void ThreeDView::handlePointerPressEvent(Platform::Application::PointerEvent& event)
{
    using Pointer = Platform::Application::Pointer;

    if (!event.isPrimary() || !(event.pointer() & (Pointer::MouseLeft)))
        return;

    const auto viewport = calculateViewport(relativeViewport_, applicationContext_.windowSize());
    if (!viewport.contains(Vector2i{event.position()}))
        return;

    viewportActive_ = true;

    Debug{} << "Viewport: " << viewport << " is active";

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

void ThreeDView::handlePointerReleaseEvent([[maybe_unused]] Platform::Application::PointerEvent& event)
{
    viewportActive_ = false;
}

void ThreeDView::handlePointerMoveEvent(Platform::Application::PointerMoveEvent& event)
{
    using Pointer  = Platform::Application::Pointer;
    using Modifier = Platform::Application::Modifier;
    using namespace Math::Literals;

    if (!event.isPrimary() || !(event.pointers() & (Pointer::MouseLeft)))
        return;

    if (!viewportActive_)
        return;

    const auto viewport = calculateViewport(relativeViewport_, applicationContext_.windowSize());
    if (!viewportActive_ && !viewport.contains(Vector2i{event.position()}))
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
        camera_->transformLocal(Matrix4::translation(rotationPoint_) * Matrix4::rotationX(-0.01_radf * delta.y())
                                * Matrix4::rotationY(-0.01_radf * delta.x()) * Matrix4::translation(-rotationPoint_));
}

void ThreeDView::handleScrollEvent(Platform::Application::ScrollEvent& event)
{
    const auto viewport = calculateViewport(relativeViewport_, applicationContext_.windowSize());
    if (!viewport.contains(Vector2i{event.position()}))
        return;
    Debug{} << "Event position is " << event.position() << " at viewport " << viewport;

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

void ThreeDView::setRelativeViewport(const Range2D& relativeViewport)
{
    CORRADE_INTERNAL_ASSERT((relativeViewport.min() >= Vector2{0.0f}).all());
    CORRADE_INTERNAL_ASSERT((relativeViewport.max() <= Vector2{1.0f}).all());

    relativeViewport_ = relativeViewport;
    viewport_         = calculateViewport(relativeViewport, applicationContext_.windowSize());
}

void ThreeDView::setViewport(const Range2Di& viewport)
{
    CORRADE_INTERNAL_ASSERT((viewport.min() >= Vector2i{0}).all());
    CORRADE_INTERNAL_ASSERT((viewport.max() <= applicationContext_.windowSize()).all());

    viewport_         = viewport;
    relativeViewport_ = calculateRelativeViewport(viewport, applicationContext_.windowSize());
}

Range2Di ThreeDView::getViewport() const
{
    return viewport_;
}

Range2D ThreeDView::calculateRelativeViewport(const Range2Di& absoluteViewport, const Vector2i& windowSize)
{
    CORRADE_INTERNAL_ASSERT((absoluteViewport.min() >= Vector2i{0}).all());
    CORRADE_INTERNAL_ASSERT((absoluteViewport.max() <= applicationContext_.windowSize()).all());

    return Range2D{Vector2{absoluteViewport.min()} / Vector2{windowSize},
                   Vector2{absoluteViewport.max()} / Vector2{windowSize}};
}

Range2Di ThreeDView::calculateViewport(const Range2D& relativeViewport, const Vector2i& windowSize)
{
    CORRADE_INTERNAL_ASSERT((relativeViewport_.min() >= Vector2{0.0f}).all());
    CORRADE_INTERNAL_ASSERT((relativeViewport_.max() <= Vector2{1.0f}).all());

    return Range2Di{relativeViewport.min() * windowSize, relativeViewport.max() * windowSize};
}

void ThreeDView::draw(SceneGraph::DrawableGroup3D& drawables)
{
    using namespace Math::Literals;

    const auto originalViewport = GL::defaultFramebuffer.viewport();

    // Convert between TL origin to BL origin (default clip space in OpenGL)
    const auto newCenter               = Vector2(relativeViewport_.center().x(), 1.0f - relativeViewport_.center().y());
    const auto flippedRelativeViewport = Range2D::fromCenter(newCenter, relativeViewport_.size() / 2.0f);

    const auto viewport = calculateViewport(flippedRelativeViewport, applicationContext_.windowSize());
    GL::defaultFramebuffer.setViewport(viewport);

    camera_->draw(drawables);

    shader_.setColor(Color3::fromHsv({35.0_degf, 1.0f, 1.0f})).draw(mesh_);

    GL::defaultFramebuffer.setViewport(originalViewport);
}
