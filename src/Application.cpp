#include "Application.h"
#include "Corrade/Utility/Debug.h"
#include "Magnum/Magnum.h"
#include "viewports/3DViewport.h"
#include "viewports/ViewportTree.h"
#include "viewports/VisibleViewport.h"

#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Image.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/MeshData.h>
#include <memory>
#include <set>

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

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    threeDView_  = std::make_unique<ThreeDView>(*this, scene_);
    threeDView1_ = std::make_unique<ThreeDView>(*this, scene_);
    grid_        = std::make_unique<Grid>(*scene_, drawables_);

    tree_ = std::make_unique<ViewportTree<ViewportNode>>(windowSize());
    viewports_[tree_->begin().get()] = std::make_unique<ThreeDViewport>(*this, tree_->begin().get(), scene_);

    // imagePreview_ = std::make_unique<ImagePreview>();

    // viewportManager_ = std::make_unique<ViewportManager>(*this, scene_);
    // viewportManager_->createNewViewport({1, 1}, ThreeDView::EBorder::LEFT);

    // viewportManager_->createNewViewport({1, 1}, ThreeDView::EBorder::BOTTOM);

    // viewportManager_->createNewViewport({1, 1}, ThreeDView::EBorder::BOTTOM);

    // viewportManager_->createNewViewport({1, 600}, ThreeDView::EBorder::TOP);

    // viewportManager_->createNewViewport({1, 600}, ThreeDView::EBorder::RIGHT);

    // viewportManager_->createNewViewport({1200, 1});
}

void CVDev::drawEvent()
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    imgui_.newFrame();

    if (ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if (!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    ImGui::Begin("Test", nullptr, window_flags);
    ImGui::Text("Hello, world!");
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.0f / io.Framerate),
                static_cast<double>(io.Framerate));
    ImGui::End();

    // threeDView1_->setViewport(Range2Di({0, 0}, {windowSize().x()/2, windowSize().y()}));
    // threeDView1_->draw(drawables_);

    // threeDView_->setViewport(Range2Di({windowSize().x()/2, 0}, {windowSize()}));
    // threeDView_->draw(drawables_);

    // viewportManager_->draw(drawables_);
    for (auto& [ptr, viewport] : viewports_)
    {
        viewport->draw(drawables_);
    }

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

void CVDev::viewportEvent(ViewportEvent& event)
{
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    imgui_.relayout(Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());

    // for (ThreeDViewport& viewport : *viewports_)
    // {
    //     viewport.setWindowSize(event.windowSize());
    // }
}

void CVDev::keyPressEvent(KeyEvent& event)
{
    if (imgui_.handleKeyPressEvent(event))
        return;

    if (event.key() == Key::E)
    {
        Utility::Debug{} << "Divide horizontally";
        ViewportNode* activeNode = tree_->findActiveViewport(Vector2i{lastMousePosition_}).get();
        viewports_.erase(activeNode);
        const auto divisionNodes = tree_->divide(Vector2i{lastMousePosition_}, PartitionDirection::HORIZONTAL);
        viewports_[divisionNodes.first] = std::make_unique<ThreeDViewport>(*this, divisionNodes.first, scene_);
        viewports_[divisionNodes.second] = std::make_unique<ThreeDViewport>(*this, divisionNodes.second, scene_);
    }
    else if (event.key() == Key::O)
    {
        Utility::Debug{} << "Divide vertically";
        ViewportNode* activeNode = tree_->findActiveViewport(Vector2i{lastMousePosition_}).get();
        viewports_.erase(activeNode);
        const auto divisionNodes = tree_->divide(Vector2i{lastMousePosition_}, PartitionDirection::VERTICAL);
        viewports_[divisionNodes.first] = std::make_unique<ThreeDViewport>(*this, divisionNodes.first, scene_);
        viewports_[divisionNodes.second] = std::make_unique<ThreeDViewport>(*this, divisionNodes.second, scene_);
    }
    // else if (event.key() == Key::LeftCtrl && event.key() == Key::LeftShift && event.key() == Key::X)
    // {

    //     tree_->collapse(Vector2i{lastMousePosition_});
    // }

    // for (auto& node : *tree_)
    // {
    //     Utility::Debug{} << node.getCoordinates();
    //     if (node.isVisible())
    //         return;
    //     Utility::Debug{} << "Erase " << node;
    //     viewports_.erase(&node);
    // }
}

void CVDev::keyReleaseEvent(KeyEvent& event)
{
    if (imgui_.handleKeyReleaseEvent(event))
        return;
}

void CVDev::pointerPressEvent(PointerEvent& event)
{
    registerMousePosition(event.position());

    if (imgui_.handlePointerPressEvent(event))
        return;

    const auto activeViewport = tree_->findActiveViewport(Vector2i{lastMousePosition_});
    viewports_.at(activeViewport.get())->interacting = activeViewport->atEdge(lastMousePosition_);
    viewports_.at(activeViewport->sibling())->interacting = viewports_.at(activeViewport.get())->interacting;

    for (auto& [ptr, viewport] : viewports_)
    {
        viewport->handlePointerPressEvent(event);
    }
    
    // viewportManager_->handlePointerPressEvent(event);
    // threeDView1_->handlePointerPressEvent(event);
    // threeDView_->handlePointerPressEvent(event);
}

void CVDev::pointerReleaseEvent(PointerEvent& event)
{
    registerMousePosition(event.position());

    if (imgui_.handlePointerReleaseEvent(event))
        return;

    for (auto& [ptr, viewport] : viewports_)
    {
        viewport->handlePointerReleaseEvent(event);
        viewport->interacting = false;
    }
    // viewportManager_->handlePointerReleaseEvent(event);
    // threeDView_->handlePointerReleaseEvent(event);
    // threeDView1_->handlePointerReleaseEvent(event);
}

void CVDev::pointerMoveEvent(PointerMoveEvent& event)
{
    registerMousePosition(event.position());

    const auto activeViewport = tree_->findActiveViewport(Vector2i{lastMousePosition_});
    if (viewports_.at(activeViewport.get())->interacting)
    {
        tree_->adjust(Vector2i{lastMousePosition_}, event.relativePosition().x());
        return;
    }

    if (imgui_.handlePointerMoveEvent(event))
        return;

    for (auto& [ptr, viewport] : viewports_)
    {
        viewport->handlePointerMoveEvent(event);
    }
    
    // viewportManager_->handlePointerMoveEvent(event);
    // threeDView1_->handlePointerMoveEvent(event);
    // threeDView_->handlePointerMoveEvent(event);
}

void CVDev::scrollEvent(ScrollEvent& event)
{
    registerMousePosition(event.position());

    if (imgui_.handleScrollEvent(event))
    {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }

    for (auto& [ptr, viewport] : viewports_)
    {
        viewport->handleScrollEvent(event);
    }

    // viewportManager_->handleScrollEvent(event);
    // threeDView1_->handleScrollEvent(event);
    // threeDView_->handleScrollEvent(event);
}

void CVDev::textInputEvent(TextInputEvent& event)
{
    if (imgui_.handleTextInputEvent(event))
        return;
}

void CVDev::registerMousePosition(const Vector2& position)
{
    // lastMousePosition_ = Vector2(position.x(), windowSize().y() - position.y());
    lastMousePosition_ = position;
}

MAGNUM_APPLICATION_MAIN(CVDev)
