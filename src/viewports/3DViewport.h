#ifndef VIEWPORTS_3DVIEWPORT_H
#define VIEWPORTS_3DVIEWPORT_H

#include "../objects/Camera.h"
#include "ViewportTree.h"
#include "VisibleViewport.h"

#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/FlatGL.h>
#include <memory>

using namespace Magnum;
class ThreeDViewport : public VisibleViewport
{
public:
    explicit ThreeDViewport(const Platform::Application&   applicationContext,
                            const ViewportNode*            node,
                            const std::shared_ptr<Scene3D> scene = std::make_shared<Scene3D>());
    virtual ~ThreeDViewport()                        = default;
    ThreeDViewport(const ThreeDViewport&)            = delete;
    ThreeDViewport(ThreeDViewport&&)                 = delete;
    ThreeDViewport& operator=(const ThreeDViewport&) = delete;
    ThreeDViewport& operator=(ThreeDViewport&&)      = delete;

    virtual void handlePointerPressEvent(Platform::Application::PointerEvent& event) override;
    virtual void handlePointerReleaseEvent(Platform::Application::PointerEvent& event) override;
    virtual void handlePointerMoveEvent(Platform::Application::PointerMoveEvent& event) override;
    virtual void handleScrollEvent(Platform::Application::ScrollEvent& event) override;
    virtual void draw(SceneGraph::DrawableGroup3D& drawables) override;

private:
    Float   lastDepth_;
    Vector2 lastPosition_{Constants::nan()};
    Vector3 rotationPoint_, translationPoint_;

    const Platform::Application& applicationContext_;
    std::shared_ptr<Scene3D>     scene_;
    std::unique_ptr<Camera>      camera_;

    // TODO: convert this into its own FlatShader class?
    GL::Mesh          mesh_;
    Shaders::FlatGL2D shader_;

    [[nodiscard]] Float   depthAt(const Vector2& windowPosition) const;
    [[nodiscard]] Vector3 unproject(const Vector2& windowPosition, Float depth) const;
};

#endif // VIEWPORTS_3DVIEWPORT_H
