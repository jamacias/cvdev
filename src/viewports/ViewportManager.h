#ifndef VIEWPORTS_VIEWPORTMANAGER_H
#define VIEWPORTS_VIEWPORTMANAGER_H

#include <memory>
#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/FlatGL.h>
#include <Magnum/SceneGraph/Drawable.h>
#include "../panels/3DView.h"

using namespace Magnum;
class ViewportManager
{
public:
    explicit ViewportManager(const Platform::Application &applicationContext, const std::shared_ptr<Scene3D> scene = std::make_shared<Scene3D>());

    void handlePointerPressEvent(Platform::Application::PointerEvent &event);
    void handlePointerReleaseEvent(Platform::Application::PointerEvent &event);
    void handlePointerMoveEvent(Platform::Application::PointerMoveEvent &event);
    void handleScrollEvent(Platform::Application::ScrollEvent &event);

    void createNewViewport(const Vector2 &position);

    void draw(SceneGraph::DrawableGroup3D &drawables);

private:    
    const Platform::Application &applicationContext_;
    std::shared_ptr<Scene3D> scene_;
    std::vector<ThreeDView> viewports_;
};

#endif // VIEWPORTS_VIEWPORTMANAGER_H