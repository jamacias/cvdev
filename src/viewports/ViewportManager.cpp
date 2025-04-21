#include "ViewportManager.h"
#include <algorithm>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/Assert.h>

ViewportManager::ViewportManager(const Platform::Application &applicationContext, const std::shared_ptr<Scene3D> scene)
: applicationContext_(applicationContext)
, scene_(scene)
{
    createNewViewport(Vector2(applicationContext_.windowSize() / 2));

    CORRADE_INTERNAL_ASSERT(viewports_.size() == 1);
}

void ViewportManager::handlePointerPressEvent(Platform::Application::PointerEvent &event)
{
    // getActiveViewport(event.position()).handlePointerPressEvent(event);
    for (auto &viewport : viewports_)
    {
        viewport.handlePointerPressEvent(event);
    }
}

void ViewportManager::handlePointerReleaseEvent(Platform::Application::PointerEvent &event)
{
    // getActiveViewport(event.position()).handlePointerReleaseEvent(event);
    for (auto &viewport : viewports_)
    {
        viewport.handlePointerReleaseEvent(event);
    }
}

void ViewportManager::handlePointerMoveEvent(Platform::Application::PointerMoveEvent &event)
{
    // getActiveViewport(event.position()).handlePointerMoveEvent(event);
    for (auto &viewport : viewports_)
    {
        viewport.handlePointerMoveEvent(event);
    }
}

void ViewportManager::handleScrollEvent(Platform::Application::ScrollEvent &event)
{
    // getActiveViewport(event.position()).handleScrollEvent(event);
    for (auto &viewport : viewports_)
    {
        viewport.handleScrollEvent(event);
    }
}

void ViewportManager::createNewViewport(const Vector2 &position)
{
    viewports_.reserve(viewports_.capacity() + 1);

    Range2Di newViewport;
    if (viewports_.empty())
    {
        Debug {} << "No viewports found, create the first one";
        newViewport = Range2Di::fromSize({0, 0}, applicationContext_.windowSize());
    }
    else
    {
        // There should be an active viewport that has to be subdivided in two and then each
        // scaled/translated to their new position.
        const auto activeViewport = std::find_if(viewports_.begin(), viewports_.end(), [&](const auto &v)
                                           { return Range2D(v.getViewport()).contains(position); });
        
                                           
        CORRADE_INTERNAL_ASSERT(activeViewport != viewports_.end());
        
        const auto activeViewportRange = Range2D(activeViewport->getViewport());

        const auto newViewportSize = activeViewportRange.scaled({0.5, 1.0}).size();
        activeViewport->setViewport(Range2Di(Range2D::fromSize(activeViewportRange.min(), newViewportSize)));

        newViewport = Range2Di(Range2D::fromSize(activeViewportRange.translated(newViewportSize * Vector2{1.0, 0.0}).min(), newViewportSize));
    }

    Debug {} << "New viewport: " << newViewport;

    auto newView = ThreeDView(applicationContext_, scene_);
    newView.setViewport(newViewport);

    viewports_.emplace_back(std::move(newView));
}

void ViewportManager::draw(SceneGraph::DrawableGroup3D &drawables)
{
    for (auto &viewport : viewports_)
    {
        viewport.draw(drawables);
    }
}