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
    enum class EBorder : uint8_t
    {
        TOP = 0,
        RIGHT,
        BOTTOM,
        LEFT
    };
    // static constexpr char * to_string(const EBorder b);
    static constexpr const char* to_string(EBorder e) noexcept
    {
        switch (e)
        {
            case EBorder::TOP:    return "TOP";
            case EBorder::RIGHT:  return "RIGHT";
            case EBorder::BOTTOM: return "BOTTOM";
            default:              return "LEFT";
        }
    }

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
    Float lastDepth_;
    Vector2 lastPosition_{Constants::nan()};
    Vector3 rotationPoint_, translationPoint_;

    const Platform::Application &applicationContext_;
    std::shared_ptr<Scene3D> scene_;
    std::unique_ptr<Camera> camera_;
    Range2Di viewport_;
    Range2D relativeViewport_; ///< Viewport relative to the current window size.
    bool viewportActive_ {false};

    // TODO: convert this into its own FlatShader class?
    GL::Mesh mesh_;
    Shaders::FlatGL2D shader_;

    [[nodiscard]] Float depthAt(const Vector2 &windowPosition);
    [[nodiscard]] Vector3 unproject(const Vector2 &windowPosition, Float depth) const;
    [[nodiscard]] Range2D calculateRelativeViewport(const Range2Di &absoluteViewport, const Vector2i &windowSize);
    [[nodiscard]] Range2Di calculateViewport(const Range2D &relativeViewport, const Vector2i &windowSize);
};

#endif // PANELS_3DVIEW_H