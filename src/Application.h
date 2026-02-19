#include "Magnum/Magnum.h"
#include "objects/Camera.h"
#include "objects/Grid.h"
#include "panels/3DView.h"
#include "panels/ImagePreview.h"
#include "traits/traits.h"
#include "viewports/3DViewport.h"
#include "viewports/ViewportManager.h"
#include "viewports/ViewportTree.h"
#include "viewports/VisibleViewport.h"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <map>
#include <memory>

using namespace Magnum;

class CVDev : public Platform::Application
{
public:
    explicit CVDev(const Arguments& arguments);

private:
    void drawEvent() override;

    void viewportEvent(ViewportEvent& event) override;

    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

    void pointerPressEvent(PointerEvent& event) override;
    void pointerReleaseEvent(PointerEvent& event) override;
    void pointerMoveEvent(PointerMoveEvent& event) override;
    void scrollEvent(ScrollEvent& event) override;
    void textInputEvent(TextInputEvent& event) override;

    void registerMousePosition(const Vector2& position);

    ImGuiIntegration::Context     imgui_{NoCreate};
    std::unique_ptr<ImagePreview> imagePreview_;

    std::shared_ptr<Scene3D>    scene_ = std::make_shared<Scene3D>();
    SceneGraph::DrawableGroup3D drawables_;

    Vector2 lastMousePosition_{};

    std::unique_ptr<Grid>            grid_;
    std::unique_ptr<ThreeDView>      threeDView_;
    std::unique_ptr<ThreeDView>      threeDView1_;
    std::unique_ptr<ViewportManager> viewportManager_;
    std::unique_ptr<ViewportTree<ViewportNode>> tree_;
    std::map<const ViewportNode*, std::unique_ptr<VisibleViewport>> viewports_; 
};
