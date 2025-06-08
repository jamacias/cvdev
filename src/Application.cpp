#include "Application.h"

#include <Magnum/GL/Renderer.h>
#include <Magnum/MeshTools/Compile.h>

#include <Magnum/Image.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/Trade/MeshData.h>

using namespace Magnum;

CVDev::CVDev(const Arguments &arguments) : Platform::Application{arguments, NoCreate}
{
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        Configuration conf;
        conf.setTitle("CVDev")
            .setWindowFlags(Configuration::WindowFlag::Resizable)
            .setSize({1280, 720}, dpiScaling);
        GLConfiguration glConf;
        glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        if (!tryCreate(conf, glConf))
            create(conf, glConf.setSampleCount(0));
    }

    using namespace Math::Literals;
    imgui_ = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(),
                                       windowSize(), framebufferSize());

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
                                   GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    threeDView_ = std::make_unique<ThreeDView>(*this, scene_);
    threeDView1_ = std::make_unique<ThreeDView>(*this, scene_);
    grid_ = std::make_unique<Grid>(*scene_, drawables_);

    imagePreview_ = std::make_unique<ImagePreview>();

    viewportManager_ = std::make_unique<ViewportManager>(*this, scene_);
    viewportManager_->createNewViewport({1, 1}, ThreeDView::EBorder::LEFT);

    viewportManager_->createNewViewport({1, 1}, ThreeDView::EBorder::BOTTOM);

    viewportManager_->createNewViewport({1, 1}, ThreeDView::EBorder::BOTTOM);
    
    viewportManager_->createNewViewport({1, 600}, ThreeDView::EBorder::TOP);
    
    viewportManager_->createNewViewport({1, 600}, ThreeDView::EBorder::RIGHT);

    // viewportManager_->createNewViewport({1200, 1});
}

void CVDev::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    imgui_.newFrame();

    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::Begin("Test", nullptr, window_flags);
    ImGui::Text("Hello, world!");
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.0f / io.Framerate), static_cast<double>(io.Framerate));
    ImGui::End();

    // threeDView1_->setViewport(Range2Di({0, 0}, {windowSize().x()/2, windowSize().y()}));
    // threeDView1_->draw(drawables_);

    // threeDView_->setViewport(Range2Di({windowSize().x()/2, 0}, {windowSize()}));
    // threeDView_->draw(drawables_);

    viewportManager_->draw(drawables_);

    // imagePreview_->draw();

    imgui_.updateApplicationCursor(*this);

    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

    imgui_.drawFrame();

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();
    redraw();
}

void CVDev::viewportEvent(ViewportEvent &event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    imgui_.relayout(Vector2{event.windowSize()} / event.dpiScaling(),
                    event.windowSize(), event.framebufferSize());
}

void CVDev::keyPressEvent(KeyEvent &event)
{
    if (imgui_.handleKeyPressEvent(event))
        return;
}

void CVDev::keyReleaseEvent(KeyEvent &event)
{
    if (imgui_.handleKeyReleaseEvent(event))
        return;
}

void CVDev::pointerPressEvent(PointerEvent &event)
{
    if (imgui_.handlePointerPressEvent(event))
        return;

    viewportManager_->handlePointerPressEvent(event);
    // threeDView1_->handlePointerPressEvent(event);
    // threeDView_->handlePointerPressEvent(event);
}

void CVDev::pointerReleaseEvent(PointerEvent &event)
{
    if (imgui_.handlePointerReleaseEvent(event))
        return;

    viewportManager_->handlePointerReleaseEvent(event);
    // threeDView_->handlePointerReleaseEvent(event);
    // threeDView1_->handlePointerReleaseEvent(event);
}

void CVDev::pointerMoveEvent(PointerMoveEvent &event)
{
    if (imgui_.handlePointerMoveEvent(event))
        return;

    viewportManager_->handlePointerMoveEvent(event);
    // threeDView1_->handlePointerMoveEvent(event);
    // threeDView_->handlePointerMoveEvent(event);
}

void CVDev::scrollEvent(ScrollEvent &event)
{
    if (imgui_.handleScrollEvent(event))
    {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }

    viewportManager_->handleScrollEvent(event);
    // threeDView1_->handleScrollEvent(event);
    // threeDView_->handleScrollEvent(event);
}

void CVDev::textInputEvent(TextInputEvent &event)
{
    if (imgui_.handleTextInputEvent(event))
        return;
}

MAGNUM_APPLICATION_MAIN(CVDev)
