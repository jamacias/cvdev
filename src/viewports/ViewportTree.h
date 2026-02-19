#ifndef VIEWPORTS_VIEWPORTTREE_H
#define VIEWPORTS_VIEWPORTTREE_H

#include "../containers/BinaryTree.h"
#include "Corrade/Utility/Assert.h"
#include "Magnum/Magnum.h"
#include "Magnum/Math/Functions.h"
#include "Magnum/Math/Vector.h"

#include <Magnum/Math/Distance.h>
#include <Magnum/Math/Range.h>
#include <algorithm>
#include <queue>

using namespace Magnum;

// Forward declarations
class ViewportNode;
template <class ViewportType>
class ViewportTree;

enum class PartitionDirection : uint8_t
{
    NONE = 0,
    HORIZONTAL,
    VERTICAL
};

class ViewportNode : public Node<ViewportNode>
{
public:
    explicit ViewportNode() = default;
    explicit ViewportNode(const Vector2i& windowSize, const Range2Di& viewport = {});
    virtual ~ViewportNode()                      = default;
    ViewportNode(const ViewportNode&)            = delete;
    ViewportNode(ViewportNode&&)                 = delete;
    ViewportNode& operator=(const ViewportNode&) = delete;
    ViewportNode& operator=(ViewportNode&&)      = delete;

    using Node<ViewportNode>::begin;
    using Node<ViewportNode>::end;

    ViewportNode& setWindowSize(const Vector2i& size);
    Vector2i      getWindowSize() const;

    ViewportNode& setRelativeCoordinates(const Range2D& coordinates);

    ViewportNode& setCoordinates(const Range2Di& coordinates);
    Range2Di      getCoordinates() const;

    void adjustPane(const Int distance);

    bool atEdge(Vector2& position) const
    {
        const auto min =
            std::min({Math::abs(coordinates_.left() - position.x()), Math::abs(coordinates_.top() - position.y()),
                      Math::abs(coordinates_.right() - position.x()), Math::abs(coordinates_.bottom() - position.y())});

        return min < 5.f;
    }

    constexpr bool isVisible() const { return isLeaf(); }

// private:
    friend ViewportTree<ViewportNode>;
    friend BinaryTree<ViewportNode>;
    friend Node<ViewportNode>;

    Vector2i           windowSize_;
    Range2Di           coordinates_;
    Range2D            relativeCoordinates_{{}, {1.0f, 1.0f}}; ///< Viewport relative to the current window size.
    Range2D            distribution_{{0.0, 0.0}, {1.0, 1.0}};  ///< How this viewport is distributed wrt to its parent.
    PartitionDirection partition_{PartitionDirection::NONE};
    void               distribute();

    [[nodiscard]] Range2D  calculateRelativeCoordinates(const Range2Di& absoluteCoordinates,
                                                        const Vector2i& windowSize) const;
    [[nodiscard]] Range2Di calculateCoordinates(const Range2D& relativeCoordinates, const Vector2i& windowSize) const;
};

template <class T>
std::pair<T&, const PartitionDirection> findClosestEdge(const Math::Vector2<T>& point, Math::Range2D<T>& coordinates)
{
    const auto leftDistance   = Math::abs(coordinates.left() - point.x());
    const auto topDistance    = Math::abs(coordinates.top() - point.y());
    const auto rightDistance  = Math::abs(coordinates.right() - point.x());
    const auto bottomDistance = Math::abs(coordinates.bottom() - point.y());

    if (leftDistance < topDistance && leftDistance < rightDistance && leftDistance < bottomDistance)
    {
        return {coordinates.left(), PartitionDirection::VERTICAL};
    }
    else if (topDistance < leftDistance && topDistance < rightDistance && topDistance < bottomDistance)
    {
        return {coordinates.top(), PartitionDirection::HORIZONTAL};
    }
    else if (rightDistance < topDistance && rightDistance < leftDistance && rightDistance < bottomDistance)
    {
        return {coordinates.right(), PartitionDirection::VERTICAL};
    }
    else
    {
        return {coordinates.bottom(), PartitionDirection::HORIZONTAL};
    }
}

template <class ViewportType>
class ViewportTree : private BinaryTree<ViewportType>
{
public:
    friend BinaryTree<ViewportType>;
    friend Node<ViewportType>;

    template<typename... Args>
    explicit ViewportTree(Args&&... args)
    : BinaryTree<ViewportType>(std::make_unique<ViewportType>(std::forward<Args>(args)...))
    {
    }
    virtual ~ViewportTree()                      = default;
    ViewportTree(const ViewportTree&)            = delete;
    ViewportTree(ViewportTree&&)                 = delete;
    ViewportTree& operator=(const ViewportTree&) = delete;
    ViewportTree& operator=(ViewportTree&&)      = delete;

    using BinaryTree<ViewportType>::begin;
    using BinaryTree<ViewportType>::end;

    BinaryTree<ViewportType>::Iterator findActiveViewport(const Vector2i& coordinates)
    {
        if ((coordinates < Vector2i{0}).all())
            return BinaryTree<ViewportType>::end();

        return std::find_if(begin(), end(), [&](const ViewportType& viewport)
                            { return viewport.getCoordinates().contains(coordinates) && viewport.isVisible(); });
    }

    std::pair<const ViewportType*, const ViewportType*> divide(const Vector2i& coordinates, const PartitionDirection& direction)
    {
        auto parent = findActiveViewport(coordinates);

        const auto parentViewport = parent->getCoordinates();
        const auto windowSize     = parent->getWindowSize();

        // Assume vertical partitioning and recalculate otherwise
        auto newViewportSize = parentViewport.size() / Vector2i(2, 1);
        auto viewport1       = Range2Di::fromSize(parentViewport.bottomLeft(), newViewportSize);
        auto viewport2       = Range2Di::fromSize(viewport1.bottomRight(), newViewportSize);
        if (direction == PartitionDirection::HORIZONTAL)
        {
            newViewportSize = parentViewport.size() / Vector2i(1, 2);
            viewport1       = Range2Di::fromSize(parentViewport.bottomLeft(), newViewportSize);
            viewport2       = Range2Di::fromSize(viewport1.topLeft(), newViewportSize);
        }

        CORRADE_INTERNAL_ASSERT(Math::join(viewport1, viewport2) == parentViewport);

        BinaryTree<ViewportType>::insert(parent, std::make_unique<ViewportType>(windowSize, Range2Di(viewport1)),
                                         std::make_unique<ViewportType>(windowSize, Range2Di(viewport2)));

        parent->partition_ = direction;
        if (direction == PartitionDirection::HORIZONTAL)
        {
            parent->left_->distribution_  = {{0.0, 0.0}, {1.0, 0.5}};
            parent->right_->distribution_ = {{0.0, 0.5}, {1.0, 1.0}};
        }
        else
        {
            parent->left_->distribution_  = {{0.0, 0.0}, {0.5, 1.0}};
            parent->right_->distribution_ = {{0.5, 0.0}, {1.0, 1.0}};
        }

        return {parent->left_.get(), parent->right_.get()};
    }

    ViewportType* collapse(const Vector2i& coordinates)
    {
        auto viewportToBeCollapsed = findActiveViewport(coordinates);
        CORRADE_INTERNAL_ASSERT(!viewportToBeCollapsed->isRoot());
        CORRADE_INTERNAL_ASSERT(viewportToBeCollapsed->isLeaf());

        /*
        Idea: reconnect the nodes and and go through the whole tree recalculating the coordinates (distribute)
        */

        const auto viewportToBeKept = static_cast<ViewportType*>(viewportToBeCollapsed->sibling());
        auto newParent = viewportToBeCollapsed->parent_->isRoot()
                             ? typename BinaryTree<ViewportType>::Iterator(nullptr)
                             : typename BinaryTree<ViewportType>::Iterator(static_cast<ViewportType*>(viewportToBeCollapsed->parent_->parent_));
        if (newParent.get())
        {
            const auto newSiblingDistribution = viewportToBeCollapsed->parent_->sibling()->distribution_;
            viewportToBeKept->distribution_   = {Vector2{1} - newSiblingDistribution.max(),
                                                 Vector2{1} - newSiblingDistribution.min()};
        }
        else // the new parent is the size of the root
        {
            viewportToBeKept->distribution_ = Range2D(Vector2{0.0f}, Vector2{1.0f});
        }

        // Relayout the tree so that the sibling of the viewport to be removed is kept
        // and linked to its grandfather.
        auto extractedViewport =
            BinaryTree<ViewportType>::cut(typename BinaryTree<ViewportType>::Iterator(viewportToBeKept));
        ViewportType* returnValue = extractedViewport.get();
        
        // TODO: we have to keep the unique ptrs because otherwise we get a segfault during insertion
        //       if compiled with optimizations. Maybe this can be improved when BinaryTree supports
        //       swapping nodes :/
        const auto nodeToBeReplaced =
            BinaryTree<ViewportType>::cut(typename BinaryTree<ViewportType>::Iterator(static_cast<ViewportType*>(viewportToBeCollapsed->parent_)));
        const auto collapsedViewport = BinaryTree<ViewportType>::cut(viewportToBeCollapsed);
        BinaryTree<ViewportType>::insert(newParent, std::move(extractedViewport));

        // Since the tree is rearranged, the screen sizes have to be re-distributed.
        // We do so level by level, otherwise we have to do many passes
        // TODO: use iterators once the BinaryTree class supports breadth-first search
        std::queue<ViewportType*> queue;
        queue.push(this->root_.get());
        while (!queue.empty())
        {
            auto node = queue.front();
            node->distribute();
            queue.pop();
            if (node->left_)
                queue.push(static_cast<ViewportType*>(node->left_.get()));
            if (node->right_)
                queue.push(static_cast<ViewportType*>(node->right_.get()));
        }

        // CORRADE_INTERNAL_ASSERT(Math::join(newParent->left_->distribution_, newParent->right_->distribution_) == Range2D(Vector2{0.0f}, Vector2{1.0f}));
        // CORRADE_INTERNAL_ASSERT(Math::join(newParent->left_->getCoordinates(), newParent->right_->getCoordinates()) == newParent->getCoordinates());

        return returnValue;
    }

    void adjust(const Vector2i& position, const Int distance)
    {
        const auto activeViewport  = findActiveViewport(position);
        const auto edge            = findClosestEdge(position, activeViewport->coordinates_);
        const auto partitionTarget = edge.second;

        ViewportNode* v                   = activeViewport.get();
        ViewportNode* longestEdgeViewport = v;
        Int           longestEdge         = 0;
        while (!v->isRoot())
        {
            if (!v->coordinates_.contains(position))
                break;

            const Int targetEdgeSize = partitionTarget == PartitionDirection::HORIZONTAL
                                           ? v->coordinates_.sizeX()
                                           : v->coordinates_.sizeY();
            if (longestEdge < targetEdgeSize)
            {
                longestEdgeViewport = v;
                longestEdge         = targetEdgeSize;
            }
            v = v->parent_;
        }

        longestEdgeViewport->adjustPane(distance);

        // TODO: use iterators once the BinaryTree class supports breadth-first search
        std::queue<ViewportType*> queue;
        queue.push(this->root_.get());
        while (!queue.empty())
        {
            auto node = queue.front();
            node->distribute();
            queue.pop();
            if (node->left_)
                queue.push(node->left_.get());
            if (node->right_)
                queue.push(node->right_.get());
        }
    }
};

#endif // VIEWPORTS_VIEWPORTTREE_H
