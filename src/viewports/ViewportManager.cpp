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
    // Check if you are close to the borders, if so, we want to move the edge of the viewport
    const auto activeViewport = std::find_if(viewports_.begin(), viewports_.end(), [&](const auto &v)
                                             { return Range2D(v.getViewport()).contains(event.position()); });

    if (activeViewport == viewports_.end())
        return;

    const auto viewport = activeViewport->getViewport();

    activatedBorder_ = findBorder(viewport, event.position());
    if (activatedBorder_)
    {
        borderInteractionViewport_ = &*activeViewport;
    }

    for (auto &viewport : viewports_)
    {
        viewport.handlePointerPressEvent(event);
    }
}

std::optional<ThreeDView::EBorder> ViewportManager::findBorder(const Range2Di &viewport, const Vector2 &position) const
{
    const Int borderActivationThreshold = 10; // px
    if (Math::abs(position.x() - viewport.left()) < borderActivationThreshold)
    {
        return ThreeDView::EBorder::LEFT;
    }
    else if (Math::abs(position.x() - viewport.right()) < borderActivationThreshold)
    {
        return ThreeDView::EBorder::RIGHT;
    }
    else if (Math::abs(position.y() - viewport.top()) < borderActivationThreshold)
    {
        return ThreeDView::EBorder::TOP;
    }
    else if (Math::abs(position.y() - viewport.bottom()) < borderActivationThreshold)
    {
        return ThreeDView::EBorder::BOTTOM;
    }

    return std::nullopt;
}

void ViewportManager::handlePointerReleaseEvent(Platform::Application::PointerEvent &event)
{
    if (activatedBorder_)
    {
        activatedBorder_ = std::nullopt;
        borderInteractionViewport_ = nullptr;
    }

    for (auto &viewport : viewports_)
    {
        viewport.handlePointerReleaseEvent(event);
    }
}

void ViewportManager::handlePointerMoveEvent(Platform::Application::PointerMoveEvent &event)
{
    if (activatedBorder_)
    {
        CORRADE_INTERNAL_ASSERT(borderInteractionViewport_ != nullptr);
        // Resize the viewport accordingly
        // TODO: what would happen if we go over to another viewport?
        const auto originalViewport = borderInteractionViewport_->getViewport();
        // Move the the edge e.g., by selecting the min
        auto newRange = originalViewport;
        // TODO: remember which edge is selected and drag that one
        const auto border = *activatedBorder_;
        if (border == ThreeDView::EBorder::LEFT)
            newRange.left() = event.position().x();
        else if (border == ThreeDView::EBorder::RIGHT)
            newRange.right() = event.position().x();
        else if (border == ThreeDView::EBorder::TOP)
            newRange.top() = event.position().y();
        else if (border == ThreeDView::EBorder::BOTTOM)
            newRange.bottom() = event.position().y();
        Debug {} << newRange;
        borderInteractionViewport_->setViewport(newRange);

        return;
    }

    for (auto &viewport : viewports_)
    {
        viewport.handlePointerMoveEvent(event);
    }
}

void ViewportManager::handleScrollEvent(Platform::Application::ScrollEvent &event)
{
    for (auto &viewport : viewports_)
    {
        viewport.handleScrollEvent(event);
    }
}

void ViewportManager::createNewViewport(const Vector2 &position, const ThreeDView::EBorder &direction)
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

        Vector2 newViewportSize;
        Vector2 newViewportTranslationFactor;
        if (direction == ThreeDView::EBorder::RIGHT || direction == ThreeDView::EBorder::LEFT)
        {
            newViewportSize = activeViewportRange.scaled({0.5, 1.0}).size();
            newViewportTranslationFactor = Vector2{1.0, 0.0};
        }
        else // == direction is top or bottom
        {
            newViewportSize = activeViewportRange.scaled({1.0, 0.5}).size();
            newViewportTranslationFactor = Vector2{0.0, 1.0};
        }

        activeViewport->setViewport(Range2Di(Range2D::fromSize(activeViewportRange.min(), newViewportSize)));
        newViewport = Range2Di(Range2D::fromSize(activeViewportRange.translated(newViewportSize * newViewportTranslationFactor).min(), newViewportSize));
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