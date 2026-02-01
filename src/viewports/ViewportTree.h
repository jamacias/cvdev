#ifndef VIEWPORTS_VIEWPORTTREE_H
#define VIEWPORTS_VIEWPORTTREE_H

#include <Magnum/Math/Range.h>
#include "../containers/BinaryTree.h"
#include "Corrade/Utility/Assert.h"
#include "Corrade/Utility/Debug.h"
#include "Magnum/Magnum.h"
#include <algorithm>
#include <queue>

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

    using Node<ViewportNode>::begin;
    using Node<ViewportNode>::end;

    ViewportNode& setWindowSize(const Vector2i &size);
    Vector2i getWindowSize() const;

    ViewportNode& setRelativeCoordinates(const Range2D &coordinates);

    ViewportNode& setCoordinates(const Range2Di &coordinates);
    Range2Di getCoordinates() const;

    constexpr bool isVisible() const { return isLeaf(); }

    void print() const
    {
        Utility::Debug{} << "this = " << this << "; &left = " << left_.get() << "; &right = " << right_.get() << "; &parent_ = " << parent_;
    
        /*
             parent
               |
             data
             /   \
          left  right
        */
        // const auto printNodeIfValid = [](ViewportNode* const n)->const char*
        //     {
        //         Utility::Debug{} << n;
        //         // return (n ? std::to_string(reinterpret_cast<uintptr_t>(n)).c_str() : "null");
        //     };
        Utility::Debug{} << "  " << parent_
                         << "\n   |\n  "
                         << this
                         << "\n  / \\\n"
                         << left_.get() << " " << right_.get()
                         ;
    }

    enum class PartitionDirection : uint8_t
    {
        NONE = 0,
        HORIZONTAL,
        VERTICAL
    };

private:
    friend ViewportTree;
    friend BinaryTree<ViewportNode>;
    friend Node<ViewportNode>;

    Vector2i windowSize_;
    Range2Di coordinates_;
    Range2D relativeCoordinates_ {{}, {1.0f, 1.0f}}; ///< Viewport relative to the current window size.
    PartitionDirection partition_ {PartitionDirection::NONE};
    Range2D distribution_{{0.0, 0.0}, {1.0, 1.0}}; ///< Viewport relative to the parent. To retrieve the size w.r.t. the main window concatenate multiplications until the root
    void distribute();

    [[nodiscard]] Range2D calculateRelativeCoordinates(const Range2Di &absoluteCoordinates, const Vector2i &windowSize) const;
    [[nodiscard]] Range2Di calculateCoordinates(const Range2D &relativeCoordinates, const Vector2i &windowSize) const;
};


class ViewportTree : private BinaryTree<ViewportNode>
{
public:
    friend BinaryTree<ViewportNode>;
    friend Node<ViewportNode>;

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

    void divide(const Vector2i& coordinates, const ViewportNode::PartitionDirection& direction)
    {
        Iterator parent = findActiveViewport(coordinates);

        const auto parentViewport = parent->getCoordinates();
        const auto windowSize = parent->getWindowSize();

        // Assume vertical partitioning and recalculate otherwise
        auto newViewportSize = parentViewport.size() / Vector2i(2, 1);
        auto viewport1 = Range2Di::fromSize(parentViewport.bottomLeft(), newViewportSize);
        auto viewport2 = Range2Di::fromSize(viewport1.bottomRight(), newViewportSize);
        if (direction == ViewportNode::PartitionDirection::HORIZONTAL)
        {
            newViewportSize = parentViewport.size() / Vector2i(1, 2);
            viewport1 = Range2Di::fromSize(parentViewport.bottomLeft(), newViewportSize);
            viewport2 = Range2Di::fromSize(viewport1.topLeft(), newViewportSize);
        }

        CORRADE_INTERNAL_ASSERT(Math::join(viewport1, viewport2) == parentViewport);

        insert(parent,
               std::make_unique<ViewportNode>(windowSize, Range2Di(viewport1)),
               std::make_unique<ViewportNode>(windowSize, Range2Di(viewport2)));

        parent->partition_ = direction;
        if (direction == ViewportNode::PartitionDirection::HORIZONTAL)
        {
            parent->left_->distribution_ = {{0.0, 0.0}, {1.0, 0.5}};
            parent->right_->distribution_ = {{0.0, 0.5}, {1.0, 1.0}};
        }
        else
        {
            parent->left_->distribution_ = {{0.0, 0.0}, {0.5, 1.0}};
            parent->right_->distribution_ = {{0.5, 0.0}, {1.0, 1.0}};
        }
    }

    void collapse([[maybe_unused]] const Vector2i& coordinates)
    {
        Iterator viewportToBeCollapsed = findActiveViewport(coordinates);
        CORRADE_INTERNAL_ASSERT(!viewportToBeCollapsed->isRoot());
        CORRADE_INTERNAL_ASSERT(viewportToBeCollapsed->isLeaf());

        /*
        Idea: reconnect the nodes and and go through the whole tree recalculating the coordinates (distribute)
        */

        const auto viewportToBeKept = viewportToBeCollapsed->sibling();
        Iterator newParent = viewportToBeCollapsed->parent_->isRoot() ? Iterator(nullptr) : Iterator(viewportToBeCollapsed->parent_->parent_);
        if (newParent.get())
        {
            const auto newSiblingDistribution = viewportToBeCollapsed->parent_->sibling()->distribution_;
            viewportToBeKept->distribution_ = {Vector2{1} - newSiblingDistribution.max(),
                                               Vector2{1} - newSiblingDistribution.min()};
        }
        else // the new parent is the size of the root
        {
            viewportToBeKept->distribution_ = Range2D(Vector2{0.0f}, Vector2{1.0f});
        }

        // Relayout the tree so that the sibling of the viewport to be removed is kept
        // and linked to its grandfather.
        auto extractedViewport = cut(Iterator(viewportToBeKept));
        cut(Iterator(viewportToBeCollapsed->parent_));
        cut(Iterator(viewportToBeCollapsed));
        insert(newParent, std::move(extractedViewport));

        // Since the tree is rearranged, the screen sizes have to be re-distributed.
        // We do so level by level, otherwise we have to do many passes
        std::queue<ViewportNode*> queue;
        queue.push(root_.get());
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

        // CORRADE_INTERNAL_ASSERT(Math::join(newParent->left_->distribution_, newParent->right_->distribution_) == Range2D(Vector2{0.0f}, Vector2{1.0f}));
        // CORRADE_INTERNAL_ASSERT(Math::join(newParent->left_->getCoordinates(), newParent->right_->getCoordinates()) == newParent->getCoordinates());
    }
};

#endif // VIEWPORTS_VIEWPORTTREE_H