#ifndef PANELS_3DVIEW_H
#define PANELS_3DVIEW_H

#include <memory>
#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/FlatGL.h>
#include <Magnum/SceneGraph/Drawable.h>
#include "../objects/Camera.h"

using namespace Magnum;
class ThreeDView
{
public:
    explicit ThreeDView(const Platform::Application &applicationContext, const std::shared_ptr<Scene3D> scene = std::make_shared<Scene3D>());

    void handlePointerPressEvent(Platform::Application::PointerEvent &event);
    void handlePointerReleaseEvent(Platform::Application::PointerEvent &event);
    void handlePointerMoveEvent(Platform::Application::PointerMoveEvent &event);
    void handleScrollEvent(Platform::Application::ScrollEvent &event);

    void setViewport(const Range2Di &viewport);

    void draw(SceneGraph::DrawableGroup3D &drawables);

private:
    Float lastDepth_;
    Vector2 lastPosition_{Constants::nan()};
    Vector3 rotationPoint_, translationPoint_;

    const Platform::Application &applicationContext_;
    std::shared_ptr<Scene3D> scene_;
    std::unique_ptr<Camera> camera_;
    Range2Di viewport_;
    bool viewportActive_ {false};

    // TODO: convert this into its own FlatShader class?
    GL::Mesh mesh_;
    Shaders::FlatGL2D shader_;

    Float depthAt(const Vector2 &windowPosition);
    Vector3 unproject(const Vector2 &windowPosition, Float depth) const;
};

#endif // PANELS_3DVIEW_H