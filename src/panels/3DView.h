#ifndef PANELS_3DVIEW_H
#define PANELS_3DVIEW_H

#include <memory>
#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Shaders/FlatGL.h>
#include <Magnum/SceneGraph/Drawable.h>
#include "../objects/Camera.h"
#include <imgui.h>

using namespace Magnum;
class ThreeDView
{
public:
    explicit ThreeDView(const Platform::Application &applicationContext, const std::shared_ptr<Scene3D> scene = std::make_shared<Scene3D>());
    // ~ThreeDView() = default;
    // ThreeDView(const ThreeDView&) = delete;
    // ThreeDView(ThreeDView&&) = delete;
    // ThreeDView& operator=(const ThreeDView&) = delete;
    // ThreeDView& operator=(ThreeDView&&) = delete;

    void handlePointerPressEvent(Platform::Application::PointerEvent &event);
    void handlePointerReleaseEvent(Platform::Application::PointerEvent &event);
    void handlePointerMoveEvent(Platform::Application::PointerMoveEvent &event);
    void handleScrollEvent(Platform::Application::ScrollEvent &event);

    void setRelativeViewport(const Range2D &viewport);
    void setViewport(const Range2Di &viewport);
    Range2D getRelativeViewport() const;
    Range2Di getViewport() const;

    void draw(SceneGraph::DrawableGroup3D &drawables);

private:
    GL::Texture2D texture_;
    Float lastDepth_;
    Vector2 lastPosition_{Constants::nan()};
    Vector3 rotationPoint_, translationPoint_;
    GL::Renderbuffer depthStencil_;

    const Platform::Application &applicationContext_;
    std::shared_ptr<Scene3D> scene_;
    std::unique_ptr<Camera> camera_;
    Range2Di viewport_;
    Range2D relativeViewport_; ///< Viewport relative to the current window size.
    bool viewportActive_ {false};

    // TODO: convert this into its own FlatShader class?
    GL::Mesh mesh_;
    Shaders::FlatGL2D shader_;

    GL::Framebuffer framebuffer_ { NoCreate };

    [[nodiscard]] Float depthAt(const Vector2 &windowPosition);
    [[nodiscard]] Vector3 unproject(const Vector2 &windowPosition, Float depth) const;
    [[nodiscard]] Range2D calculateRelativeViewport(const Range2Di &absoluteViewport, const Vector2i &windowSize);
    [[nodiscard]] Range2Di calculateViewport(const Range2D &relativeViewport, const Vector2i &windowSize);

    void pan(const Vector2 &position);
    void orbit(const Vector2 &position);
    void scroll(const Vector2 &position, const Vector2 &scrollOffset);
    bool interactionActive_ {false};

    Int instanceID_ {-1};
    static Int instancesCount_;
};

#endif // PANELS_3DVIEW_H