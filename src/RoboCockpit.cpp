#include "RoboCockpit.h"

#include <Magnum/GL/Renderer.h>
#include <Magnum/Ui/Style.h>
#include <Magnum/Ui/SnapLayouter.h>
#include <Magnum/Ui/Button.h>
#include <Magnum/Ui/BaseLayer.h>
#include "Magnum/Ui/Application.h"

using namespace Magnum;

RoboCockpit::RoboCockpit(const Arguments &arguments) : Platform::Application{arguments, NoCreate}
{
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        Configuration conf;
        conf.setTitle("RoboCockpit")
            .setWindowFlags(Configuration::WindowFlag::Resizable)
            .setSize({1280, 720}, dpiScaling);
        GLConfiguration glConf;
        glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        if (!tryCreate(conf, glConf))
            create(conf, glConf.setSampleCount(0));
    }

    ui_.create(*this, Ui::McssDarkStyle{}, Ui::StyleFeature::BaseLayer);
    Ui::NodeHandle root = ui_.createNode({}, ui_.size());    
    auto& baseLayer = ui_.baseLayer();
    baseLayer.create(4, root);

    using namespace Math::Literals;
    
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
                                   GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);
}

void RoboCockpit::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    ui_.draw();

    swapBuffers();
    if(ui_.state())
        redraw();
}

void RoboCockpit::viewportEvent(ViewportEvent &event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    ui_.setSize(event);
}

void RoboCockpit::keyPressEvent([[maybe_unused]] KeyEvent &event)
{
    ui_.keyPressEvent(event);

    if(ui_.state())
        redraw();
}

void RoboCockpit::keyReleaseEvent([[maybe_unused]] KeyEvent &event)
{
    ui_.keyReleaseEvent(event);

    if(ui_.state())
        redraw();
}

void RoboCockpit::pointerPressEvent([[maybe_unused]] PointerEvent &event)
{
    ui_.pointerPressEvent(event);

    if(ui_.state())
        redraw();
}

void RoboCockpit::pointerReleaseEvent([[maybe_unused]] PointerEvent &event)
{
    ui_.pointerReleaseEvent(event);

    if(ui_.state())
        redraw();
}

void RoboCockpit::pointerMoveEvent([[maybe_unused]] PointerMoveEvent &event)
{
    // ui_.pointerMoveEvent(event);

    if(ui_.state())
        redraw();
}

void RoboCockpit::scrollEvent([[maybe_unused]] ScrollEvent &event)
{
    ui_.scrollEvent(event);

    if(ui_.state())
        redraw();
}

void RoboCockpit::textInputEvent([[maybe_unused]] TextInputEvent &event)
{
    ui_.textInputEvent(event);

    if(ui_.state())
        redraw();
}

MAGNUM_APPLICATION_MAIN(RoboCockpit)
