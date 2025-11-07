#ifndef VIEWPORTS_VIEWPORTTREE_H
#define VIEWPORTS_VIEWPORTTREE_H

#include <Magnum/Math/Range.h>
#include "../containers/BinaryTree.h"
#include "AbstractViewport.h"

using namespace Magnum;

// Forward declarations
class ViewportNode;
class ViewportTree;

// TODO: move all the Ranges here
// AbstractViewport shall have the pointers interaction instead and inherit from this
class ViewportNode : private Node<ViewportNode>
{
public:
    explicit ViewportNode() = default;
    explicit ViewportNode(const Vector2i &windowSize, const Range2Di &viewport = {});
    virtual ~ViewportNode() = default;
    ViewportNode(const ViewportNode&) = delete;
    ViewportNode(ViewportNode&&) = delete;
    ViewportNode& operator=(const ViewportNode&) = delete;
    ViewportNode& operator=(ViewportNode&&) = delete;

    ViewportNode& setWindowSize(const Vector2i &size);
    Vector2i getWindowSize() const;

    ViewportNode& setRelativeCoordinates(const Range2D &coordinates);

    ViewportNode& setCoordinates(const Range2Di &coordinates);
    Range2Di getCoordinates() const;

    constexpr bool isVisible() const { return isLeaf(); }

    friend ViewportTree;
    friend BinaryTree<ViewportNode>;

private:
    Vector2i windowSize_;
    Range2Di coordinates_;
    Range2D relativeCoordinates_ {{}, {1.0f, 1.0f}}; ///< Viewport relative to the current window size.

    [[nodiscard]] Range2D calculateRelativeCoordinates(const Range2Di &absoluteCoordinates, const Vector2i &windowSize) const;
    [[nodiscard]] Range2Di calculateCoordinates(const Range2D &relativeCoordinates, const Vector2i &windowSize) const;
};


class ViewportTree : private BinaryTree<ViewportNode>
{
public:
    enum class PartitionDirection : uint8_t
    {
        HORIZONTAL = 0,
        VERTICAL
    };

    explicit ViewportTree(const Vector2i &windowSize)
    : BinaryTree(std::make_unique<ViewportNode>(windowSize))
    {

    }
    virtual ~ViewportTree() = default;
    ViewportTree(const ViewportTree&) = delete;
    ViewportTree(ViewportTree&&) = delete;
    ViewportTree& operator=(const ViewportTree&) = delete;
    ViewportTree& operator=(ViewportTree&&) = delete;

    using BinaryTree<ViewportNode>::begin;
    using BinaryTree<ViewportNode>::end;

    Iterator findActiveViewport(const Vector2i& coordinates)
    {
        if ((coordinates < Vector2i{0}).all())
            return end();

        return std::find_if(begin(), end(), [&](const ViewportNode &viewport)
                            { return viewport.getCoordinates().contains(coordinates) && viewport.isVisible(); });
    }

    void divide(const Vector2i& coordinates, const PartitionDirection& direction)
    {
        Iterator parent = findActiveViewport(coordinates);

        const auto parentViewport = parent->getCoordinates();
        const auto windowSize = parent->getWindowSize();

        // Assume vertical partitioning and recalculate otherwise
        auto newViewportSize = parentViewport.size() / Vector2i(2, 1);
        auto viewport1 = Range2Di::fromSize(parentViewport.bottomLeft(), newViewportSize);
        auto viewport2 = Range2Di::fromSize(viewport1.bottomRight(), newViewportSize);
        if (direction == PartitionDirection::HORIZONTAL)
        {
            newViewportSize = parentViewport.size() / Vector2i(1, 2);
            viewport1 = Range2Di::fromSize(parentViewport.bottomLeft(), newViewportSize);
            viewport2 = Range2Di::fromSize(viewport1.topLeft(), newViewportSize);
        }

        CORRADE_INTERNAL_ASSERT(Math::join(viewport1, viewport2) == parentViewport);

        insert(parent,
               std::make_unique<ViewportNode>(windowSize, Range2Di(viewport1)),
               std::make_unique<ViewportNode>(windowSize, Range2Di(viewport2)));
    }

    void merge([[maybe_unused]] const Vector2i& coordinates)
    {
        // Iterator parent = findActiveViewport(coordinates);

        // TODO: implement imbalanced trees (e.g., exchange nodes or extract one to be merged into the parent).
        // move the child node to the parent somehow
    }
};

#endif // VIEWPORTS_VIEWPORTTREE_H