#ifndef VIEWPORTS_VIEWPORTTREE_H
#define VIEWPORTS_VIEWPORTTREE_H

#include <Magnum/Math/Range.h>
#include "../containers/BinaryTree.h"
#include "AbstractViewport.h"
#include <string>

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
    }

    void collapse([[maybe_unused]] const Vector2i& coordinates)
    {
        Iterator activeViewport = findActiveViewport(coordinates);
        CORRADE_INTERNAL_ASSERT(!activeViewport->isRoot());
        CORRADE_INTERNAL_ASSERT(!activeViewport->parent_->isRoot());
        CORRADE_INTERNAL_ASSERT(activeViewport->isLeaf());

        Utility::Debug{} << "Active viewport coords: " << activeViewport->getCoordinates();

        const auto newParent = Iterator(activeViewport->parent_->parent_);
        // newParent->print();

        Utility::Debug{} << "New parent coords: " << newParent->getCoordinates();

        // Calculate the coordinates of the parent and distribute them to the children proportionally to their current values
        const auto activeViewportParentCoords = activeViewport->parent_->getCoordinates();
        Utility::Debug{} << "Active viewport parent coords: " << activeViewportParentCoords;
        
        // Calculate the resulting delta needed to be applied to every children (viewport). This is, the (absolute) difference between the sibling of the
        // viewport to be removed and its parent since the sum of both childrens must always be equal to that of its parent.
        const auto activeViewportSibling = activeViewport->sibling();
        const Range2Di delta{Math::abs(activeViewportParentCoords.min() - activeViewportSibling->getCoordinates().min()),
                             Math::abs(activeViewportParentCoords.max() - activeViewportSibling->getCoordinates().max())};
        Utility::Debug{} << "Delta: " << delta;

        // Now we have to iterate all the children of the extracted viewport and recalculate their coordinates proportionally
        activeViewportSibling->setCoordinates(activeViewportParentCoords);
        for (auto &node : *activeViewportSibling)
        {
            if (&node == activeViewportSibling) // the coordinates of the sibling of the viewport to be removed are already set
                continue;

            const auto originalCoords = node.getCoordinates();
            const Range2Di newCoords{originalCoords.min() - delta.min(),
                                     originalCoords.max() - delta.max()};
            Utility::Debug{} << "New coords: " << newCoords << "; from: " << originalCoords;
            node.setCoordinates(newCoords);
        }

        // Relayout the tree so that the sibling of the viewport to be removed is kept
        // and linked to its grandfather.
        auto extractedViewport = cut(Iterator(activeViewportSibling));
        cut(Iterator(activeViewport->parent_));
        cut(Iterator(activeViewport));
        insert(newParent, std::move(extractedViewport));

        Utility::Debug{} << "Leave collapse function";

        CORRADE_INTERNAL_ASSERT(Math::join(newParent->left_->getCoordinates(), newParent->right_->getCoordinates()) == newParent->getCoordinates());
    }
};

#endif // VIEWPORTS_VIEWPORTTREE_H