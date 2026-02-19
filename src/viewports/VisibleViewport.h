#ifndef VIEWPORTS_VISIBLEVIEWPORT_H
#define VIEWPORTS_VISIBLEVIEWPORT_H

#include "ViewportTree.h"

#include <Magnum/Math/Range.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Drawable.h>

using namespace Magnum;
class VisibleViewport
{
public:
    explicit VisibleViewport(const ViewportNode* node)
    : node_(node)
    {
    }
    virtual ~VisibleViewport()                         = default;
    VisibleViewport(const VisibleViewport&)            = delete;
    VisibleViewport(VisibleViewport&&)                 = delete;
    VisibleViewport& operator=(const VisibleViewport&) = delete;
    VisibleViewport& operator=(VisibleViewport&&)      = delete;

    virtual void handlePointerPressEvent(Platform::Application::PointerEvent& event)    = 0;
    virtual void handlePointerReleaseEvent(Platform::Application::PointerEvent& event)  = 0;
    virtual void handlePointerMoveEvent(Platform::Application::PointerMoveEvent& event) = 0;
    virtual void handleScrollEvent(Platform::Application::ScrollEvent& event)           = 0;
    virtual void draw(SceneGraph::DrawableGroup3D& drawables)                           = 0;

    [[nodiscard]] bool isActive(const Vector2& coordinates) const
    {
        return node_->getCoordinates().contains(Vector2i{coordinates});
    }
    bool interacting {false};

protected:
    const ViewportNode* node_ {nullptr};
};

#endif // VIEWPORTS_VISIBLEVIEWPORT_H
