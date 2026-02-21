#ifndef VIEWPORTS_3DVIEWPORT_H
#define VIEWPORTS_3DVIEWPORT_H

#include "../objects/Camera.h"
#include "Magnum/Magnum.h"

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/FlatGL.h>
#include <memory>

using namespace Magnum;
class ThreeDViewport
{
public:
    explicit ThreeDViewport(const Range2Di&                viewportArea,
                            const std::shared_ptr<Scene3D> scene = std::make_shared<Scene3D>());
    virtual ~ThreeDViewport()                        = default;
    ThreeDViewport(const ThreeDViewport&)            = delete;
    ThreeDViewport(ThreeDViewport&&)                 = delete;
    ThreeDViewport& operator=(const ThreeDViewport&) = delete;
    ThreeDViewport& operator=(ThreeDViewport&&)      = delete;

    void handlePointerPressEvent(Platform::Application::PointerEvent& event);
    void handlePointerReleaseEvent(Platform::Application::PointerEvent& event);
    void handlePointerMoveEvent(Platform::Application::PointerMoveEvent& event);
    void handleScrollEvent(Platform::Application::ScrollEvent& event);
    void draw(SceneGraph::DrawableGroup3D& drawables);

private:
    Float   lastDepth_;
    Vector2 lastPosition_{Constants::nan()};
    Vector3 rotationPoint_, translationPoint_;

    Range2Di                 viewportArea_;
    std::shared_ptr<Scene3D> scene_;
    std::unique_ptr<Camera>  camera_;

    Magnum::GL::Framebuffer  framebuffer_{NoCreate};
    Magnum::GL::Texture2D    colorTexture_{NoCreate};
    Magnum::GL::Renderbuffer depthBuffer_{NoCreate};

    // TODO: convert this into its own FlatShader class?
    GL::Mesh          mesh_;
    Shaders::FlatGL2D shader_;

    [[nodiscard]] Float   depthAt(const Vector2& windowPosition);
    [[nodiscard]] Vector3 unproject(const Vector2& windowPosition, Float depth) const;
};

#endif // VIEWPORTS_3DVIEWPORT_H
