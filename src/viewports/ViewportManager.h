#ifndef VIEWPORTS_VIEWPORTMANAGER_H
#define VIEWPORTS_VIEWPORTMANAGER_H

#include <memory>
#include <optional>
#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/FlatGL.h>
#include <Magnum/SceneGraph/Drawable.h>
#include "../panels/3DView.h"
#include "../containers/BinaryTree.h"

using namespace Magnum;
class ViewportManager
{
public:
    explicit ViewportManager(const Platform::Application &applicationContext, const std::shared_ptr<Scene3D> scene = std::make_shared<Scene3D>());

    void handlePointerPressEvent(Platform::Application::PointerEvent &event);
    void handlePointerReleaseEvent(Platform::Application::PointerEvent &event);
    void handlePointerMoveEvent(Platform::Application::PointerMoveEvent &event);
    void handleScrollEvent(Platform::Application::ScrollEvent &event);

    void createNewViewport(const Vector2 &position, const ThreeDView::EBorder &direction = ThreeDView::EBorder::LEFT);

    void draw(SceneGraph::DrawableGroup3D &drawables);

private:
    std::optional<ThreeDView::EBorder> findBorder(const Range2Di &viewport, const Vector2 &position) const;
    const Platform::Application &applicationContext_;
    std::shared_ptr<Scene3D> scene_;
    std::vector<ThreeDView> viewports_;
    std::optional<ThreeDView::EBorder> activatedBorder_ {std::nullopt};
    ThreeDView* borderInteractionViewport_ {nullptr};
};

#endif // VIEWPORTS_VIEWPORTMANAGER_H