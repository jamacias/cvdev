#include "ViewportTree.h"

#include "Corrade/Utility/Assert.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Range.h"

ViewportNode::ViewportNode(const Vector2i& windowSize, const Range2Di& coordinates)
{
    windowSize_ = windowSize;

    if (coordinates == Range2Di{})
        setCoordinates({{0, 0}, windowSize});
    else
        setCoordinates(coordinates);
}

ViewportNode& ViewportNode::setWindowSize(const Vector2i& size)
{
    windowSize_  = size;
    coordinates_ = calculateCoordinates(relativeCoordinates_, windowSize_);

    return *this;
}

Vector2i ViewportNode::getWindowSize() const
{
    return windowSize_;
}

ViewportNode& ViewportNode::setRelativeCoordinates(const Range2D& relativeCoordinates)
{
    CORRADE_INTERNAL_ASSERT((relativeCoordinates.min() >= Vector2{0.0f}).all());
    CORRADE_INTERNAL_ASSERT((relativeCoordinates.max() <= Vector2{1.0f}).all());

    relativeCoordinates_ = relativeCoordinates;
    coordinates_         = calculateCoordinates(relativeCoordinates, windowSize_);

    return *this;
}

ViewportNode& ViewportNode::setCoordinates(const Range2Di& coordinates)
{
    CORRADE_INTERNAL_ASSERT((coordinates.min() >= Vector2i{0}).all());
    CORRADE_INTERNAL_ASSERT((coordinates.max() <= windowSize_).all());

    coordinates_         = coordinates;
    relativeCoordinates_ = calculateRelativeCoordinates(coordinates, windowSize_);

    return *this;
}

Range2Di ViewportNode::getCoordinates() const
{
    return coordinates_;
}

void ViewportNode::adjustPane(const Int distance)
{
    if (isRoot())
        return;

    const Vector2 deltaMask = distribution_.x().size() != 1.0f ? Vector2{1, 0} : Vector2{0, 1};
    const Vector2 distributionDelta =
        deltaMask * Math::lerpInverted(Vector2{parent_->coordinates_.min()}, Vector2{parent_->coordinates_.max()},
                                       Vector2{static_cast<float>(distance)});

    const auto moveEdge = [](Range2D& distribution, const Float distance)
    {
        if (auto& top = distribution.top(); top != 0.0f && top != 1.0f)
        {
            top += distance;
        }
        else if (auto& right = distribution.right(); right != 0.0f && right != 1.0f)
        {
            right += distance;
        }
        else if (auto& bottom = distribution.bottom(); bottom != 0.0f && bottom != 1.0f)
        {
            bottom += distance;
        }
        else if (auto& left = distribution.left(); left != 0.0f && left != 1.0f)
        {
            left += distance;
        }
        else
        {
            CORRADE_ASSERT_UNREACHABLE(
                "[ViewportNode::adjustPane::moveEdge] Cannot move edges that are already at the borders.", {});
        }
    };

    moveEdge(distribution_, distributionDelta.x() != 0.0f ? distributionDelta.x() : distributionDelta.y());
    moveEdge(sibling()->distribution_, distributionDelta.x() != 0.0f ? distributionDelta.x() : distributionDelta.y());
}

void ViewportNode::distribute()
{
    if (isRoot())
    {
        CORRADE_INTERNAL_ASSERT(distribution_ == Range2D(Vector2{0.0f}, Vector2{1.0f}));
        setCoordinates(Range2Di(Vector2i(0), windowSize_));

        return;
    }

    const auto&    parentCoordinates = parent_->coordinates_;
    const Range2Di newCoordinates{Math::lerp(parentCoordinates.min(), parentCoordinates.max(), distribution_.min()),
                                  Math::lerp(parentCoordinates.min(), parentCoordinates.max(), distribution_.max())};
    setCoordinates(newCoordinates);
}

Range2D ViewportNode::calculateRelativeCoordinates(const Range2Di& absoluteViewport, const Vector2i& windowSize) const
{
    CORRADE_INTERNAL_ASSERT((absoluteViewport.min() >= Vector2i{0}).all());
    CORRADE_INTERNAL_ASSERT((absoluteViewport.max() <= windowSize).all());
    CORRADE_INTERNAL_ASSERT((windowSize > Vector2i{0}).all());

    return Range2D{Vector2{absoluteViewport.min()} / Vector2{windowSize},
                   Vector2{absoluteViewport.max()} / Vector2{windowSize}};
}

Range2Di ViewportNode::calculateCoordinates(const Range2D& relativeCoordinates, const Vector2i& windowSize) const
{
    CORRADE_INTERNAL_ASSERT((relativeCoordinates_.min() >= Vector2{0.0f}).all());
    CORRADE_INTERNAL_ASSERT((relativeCoordinates_.max() <= Vector2{1.0f}).all());

    return Range2Di{relativeCoordinates.min() * windowSize, relativeCoordinates.max() * windowSize};
}
