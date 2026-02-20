#include "Application.h"
#include "viewports/3DViewport.h"

#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Image.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/MeshData.h>
#include <memory>

using namespace Magnum;

CVDev::CVDev(const Arguments& arguments)
: Platform::Application{arguments, NoCreate}
{
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        Configuration conf;
        conf.setTitle("CVDev").setWindowFlags(Configuration::WindowFlag::Resizable).setSize({1280, 720}, dpiScaling);
        GLConfiguration glConf;
        glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        if (!tryCreate(conf, glConf))
            create(conf, glConf.setSampleCount(0));
    }

    using namespace Math::Literals;
    imgui_ = ImGuiIntegration::Context(Vector2{windowSize()} / dpiScaling(), windowSize(), framebufferSize());
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    grid_ = std::make_unique<Grid>(*scene_, drawables_);
    viewport_ = std::make_unique<ThreeDViewport>(windowSize(), scene_);
}

void CVDev::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    imgui_.newFrame();

    if (ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if (!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    const ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | 
                                               ImGuiDockNodeFlags_AutoHideTabBar;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspace_flags);

    const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::Begin("Test", nullptr, window_flags);
    ImGui::Text("Hello, world!");
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.0f / io.Framerate),
                static_cast<double>(io.Framerate));
    ImGui::End();

    viewport_->draw(drawables_);

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

void CVDev::viewportEvent(ViewportEvent& event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    imgui_.relayout(Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());
}

void CVDev::keyPressEvent(KeyEvent& event)
{
    if (imgui_.handleKeyPressEvent(event))
        return;
}

void CVDev::keyReleaseEvent(KeyEvent& event)
{
    if (imgui_.handleKeyReleaseEvent(event))
        return;
}

void CVDev::pointerPressEvent(PointerEvent& event)
{
    if (imgui_.handlePointerPressEvent(event))
        return;
}

void CVDev::pointerReleaseEvent(PointerEvent& event)
{
    if (imgui_.handlePointerReleaseEvent(event))
        return;
}

void CVDev::pointerMoveEvent(PointerMoveEvent& event)
{
    if (imgui_.handlePointerMoveEvent(event))
        return;
}

void CVDev::scrollEvent(ScrollEvent& event)
{
    if (imgui_.handleScrollEvent(event))
    {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }
}

void CVDev::textInputEvent(TextInputEvent& event)
{
    if (imgui_.handleTextInputEvent(event))
        return;
}

MAGNUM_APPLICATION_MAIN(CVDev)
