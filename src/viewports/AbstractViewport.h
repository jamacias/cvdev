#ifndef VIEWPORTS_ABSTRACTVIEWPORT_H
#define VIEWPORTS_ABSTRACTVIEWPORT_H

#include "../containers/BinaryTree.h"
#include "../traits/traits.h"

#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <memory>

using namespace Magnum;
class AbstractViewport : public Node<AbstractViewport>
{
public:
    explicit AbstractViewport(const Vector2i& windowSize = {1, 1}, const Range2Di& viewport = {});
    virtual ~AbstractViewport()                          = default;
    AbstractViewport(const AbstractViewport&)            = delete;
    AbstractViewport(AbstractViewport&&)                 = delete;
    AbstractViewport& operator=(const AbstractViewport&) = delete;
    AbstractViewport& operator=(AbstractViewport&&)      = delete;

    virtual void handlePointerPressEvent(Platform::Application::PointerEvent& event)    = 0;
    virtual void handlePointerReleaseEvent(Platform::Application::PointerEvent& event)  = 0;
    virtual void handlePointerMoveEvent(Platform::Application::PointerMoveEvent& event) = 0;
    virtual void handleScrollEvent(Platform::Application::ScrollEvent& event)           = 0;
    virtual void draw(SceneGraph::DrawableGroup3D& drawables)                           = 0;

    AbstractViewport& setWindowSize(const Vector2i& size);

    AbstractViewport& setRelativeViewport(const Range2D& viewport);

    AbstractViewport& setViewport(const Range2Di& viewport);
    Range2Di          getViewport() const;

private:
    Vector2i windowSize_;
    Range2Di viewport_;
    Range2D  relativeViewport_; ///< Viewport relative to the current window size.
    bool     viewportActive_{false};

    [[nodiscard]] Range2D calculateRelativeViewport(const Range2Di& absoluteViewport, const Vector2i& windowSize) const;
    [[nodiscard]] Range2Di calculateViewport(const Range2D& relativeViewport, const Vector2i& windowSize) const;
};

#endif // VIEWPORTS_ABSTRACTVIEWPORT_H
