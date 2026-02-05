#include "AbstractViewport.h"

AbstractViewport::AbstractViewport(const Vector2i &windowSize, const Range2Di &viewport)
{
    windowSize_ = windowSize;
    setViewport(viewport);
}

AbstractViewport& AbstractViewport::setWindowSize(const Vector2i &size)
{
    windowSize_ = size;
    viewport_ = calculateViewport(relativeViewport_, windowSize_);

    return *this;
}

AbstractViewport& AbstractViewport::setRelativeViewport(const Range2D &relativeViewport)
{
    CORRADE_INTERNAL_ASSERT((relativeViewport.min() >= Vector2{0.0f}).all());
    CORRADE_INTERNAL_ASSERT((relativeViewport.max() <= Vector2{1.0f}).all());

    relativeViewport_ = relativeViewport;
    viewport_ = calculateViewport(relativeViewport, windowSize_);

    return *this;
}

AbstractViewport& AbstractViewport::setViewport(const Range2Di &viewport)
{
    CORRADE_INTERNAL_ASSERT((viewport.min() >= Vector2i{0}).all());
    CORRADE_INTERNAL_ASSERT((viewport.max() <= windowSize_).all());
    
    viewport_ = viewport;
    relativeViewport_ = calculateRelativeViewport(viewport, windowSize_);

    return *this;
}

Range2Di AbstractViewport::getViewport() const
{
    return viewport_;
}

Range2D AbstractViewport::calculateRelativeViewport(const Range2Di &absoluteViewport, const Vector2i &windowSize) const
{
    CORRADE_INTERNAL_ASSERT((absoluteViewport.min() >= Vector2i{0}).all());
    CORRADE_INTERNAL_ASSERT((absoluteViewport.max() <= windowSize).all());
    CORRADE_INTERNAL_ASSERT((windowSize > Vector2i{0}).all());

    return Range2D{Vector2{absoluteViewport.min()} / Vector2{windowSize}, Vector2{absoluteViewport.max()} / Vector2{windowSize}};
}

Range2Di AbstractViewport::calculateViewport(const Range2D &relativeViewport, const Vector2i &windowSize) const
{
    CORRADE_INTERNAL_ASSERT((relativeViewport_.min() >= Vector2{0.0f}).all());
    CORRADE_INTERNAL_ASSERT((relativeViewport_.max() <= Vector2{1.0f}).all());

    return Range2Di{relativeViewport.min() * windowSize, relativeViewport.max() * windowSize};
}
