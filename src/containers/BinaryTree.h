#ifndef CONTAINERS_BINARYTREE_H
#define CONTAINERS_BINARYTREE_H

#include <memory>

template <class Derived>
class Node;

template <class T>
class BinaryTree
{
public:
    constexpr explicit BinaryTree(std::unique_ptr<T> root = nullptr)
    : root_(std::move(root))
    {
        if (root_)
            size_++;
    }

    BinaryTree(const BinaryTree<T>&) = delete;
    BinaryTree<T>(BinaryTree<T>&& other) { *this = std::move(other); }
    BinaryTree<T>& operator=(const BinaryTree<T>&) = delete;
    BinaryTree<T>& operator=(BinaryTree<T>&& other)
    {
        root_ = std::move(other.root_);
        size_ = other.size_;

        other.root_ = nullptr;
        other.size_ = 0;

        return *this;
    }
    virtual ~BinaryTree() = default;

    template <class I>
    class iterator;
    using Iterator      = iterator<T>;
    using ConstIterator = iterator<const T>;

    template <class I>
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = I;
        using pointer           = value_type*;
        using reference         = value_type&;
        constexpr explicit iterator(pointer node)
        : node_(node)
        {
        }

        constexpr reference operator*() const { return *node_; }

        constexpr pointer operator->() const { return node_; }

        constexpr bool operator!=(const iterator& other) const { return node_ != other.node_; }

        iterator& operator++()
        {
            node_ = Node<T>::next(node_);
            return *this;
        }

        constexpr pointer get() const { return node_; }

    private:
        pointer node_{nullptr};
    };

    Iterator begin() { return root_ ? Iterator(Node<T>::leftMost(root_.get())) : end(); }
    Iterator end() { return Iterator(nullptr); }

    constexpr ConstIterator begin() const { return root_ ? ConstIterator(Node<T>::leftMost(root_.get())) : end(); }
    constexpr ConstIterator end() const { return ConstIterator(nullptr); }

    constexpr void insert(Iterator parent, std::unique_ptr<T> left, std::unique_ptr<T> right)
    {
        CORRADE_INTERNAL_ASSERT(parent.get() && (left || right));
        CORRADE_ASSERT((left && !parent->left_) || (right && !parent->right_),
                       "BinaryTree::insert(): the parent cannot already have children", );
        // At the bottom => append if it is a leaf

        if (left)
        {
            left->parent_ = parent.get();
            parent->left_ = std::move(left);
            size_ += 1;
        }

        if (right)
        {
            right->parent_ = parent.get();
            parent->right_ = std::move(right);
            size_ += 1;
        }

        // TODO:
        // At the top => move the root node
        // At the middle => move nodes and insert in the middle
    }

    constexpr void insert(Iterator parent, std::unique_ptr<T> node)
    {
        // TODO: test inserting one node at the root
        CORRADE_INTERNAL_ASSERT(node);
        if (!parent.get())
        {
            root_ = std::move(node);
            return;
        }

        CORRADE_ASSERT((parent->left_ && !parent->right_) || (!parent->left_ && parent->right_),
                       "BinaryTree::insert(): the parent should only have one children. Use the other overloads if you "
                       "only want to insert one node.", );

        if (parent->left_)
            insert(parent, nullptr, std::move(node));
        else
            insert(parent, std::move(node), nullptr);
    }

    constexpr void remove(Iterator node)
    {
        if (!node.get())
            return;

        T* nextPtr{nullptr};
        do
        {
            T* current = Node<T>::leftMost(node->parent_)->parent_;

            if (current->left_)
                current->left_ = nullptr;
            if (current->right_)
                current->right_ = nullptr;

            size_ -= 2;

            nextPtr = Node<T>::next(current);
        } while (nextPtr != nullptr && nextPtr->isLeaf() && nextPtr != node->parent_);

        remove(Iterator(node->parent_->right_.get()));

        if (node->isLeaf())
        {
            size_ -= 2;
        }
    }

    constexpr std::unique_ptr<T> cut(Iterator node)
    {
        CORRADE_INTERNAL_ASSERT(node.get());

        if (node->isRoot())
            return std::move(root_);

        std::unique_ptr<T> returnNode{nullptr};
        if (node->parent_->left_.get() == node.get()) // Node to cut is left
        {
            returnNode = std::move(node->parent_->left_);
            --size_;
        }
        else if (node->parent_->right_.get() == node.get()) // Node to cut is right
        {
            returnNode = std::move(node->parent_->right_);
            --size_;
        }

        node->parent_ = nullptr;

        return returnNode;
    }

    constexpr std::size_t size() const { return size_; }

protected:
    std::unique_ptr<T> root_{nullptr};

private:
    std::size_t size_{0};
};

template <class Derived>
class Node
{
    friend BinaryTree<Derived>;

public:
    constexpr explicit Node()
    : left_(nullptr)
    , right_(nullptr)
    , parent_(nullptr)
    {
    }
    Node(const Node<Derived>&)                      = delete;
    Node<Derived>(Node<Derived>&& other)            = delete;
    Node<Derived>& operator=(const Node<Derived>&)  = delete;
    Node<Derived>& operator=(Node<Derived>&& other) = delete;
    virtual ~Node()                                 = default;

    constexpr bool     isRoot() const { return !parent_; }
    constexpr bool     isLeaf() const { return !left_ && !right_; }
    constexpr Derived* sibling() const
    {
        CORRADE_INTERNAL_ASSERT(!isRoot());

        Derived* sibling{nullptr};
        if (parent_->left_.get() == this)
            sibling = parent_->right_.get();
        else if (parent_->right_.get() == this)
            sibling = parent_->left_.get();

        return sibling;
    }

    template <class I>
    class iterator;
    using Iterator      = iterator<Derived>;
    using ConstIterator = iterator<const Derived>;

    template <class I>
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = I;
        using pointer           = value_type*;
        using reference         = value_type&;
        constexpr explicit iterator(pointer node)
        : node_(node)
        {
        }

        constexpr reference operator*() const { return *node_; }

        constexpr pointer operator->() const { return node_; }

        constexpr bool operator!=(const iterator& other) const { return node_ != other.node_; }

        iterator& operator++()
        {
            node_ = next(node_);
            return *this;
        }

        constexpr pointer get() const { return node_; }

    private:
        pointer node_{nullptr};
    };

    Iterator begin() { return Iterator(leftMost(static_cast<Derived*>(this))); }
    Iterator end() { return std::next(Iterator(rightMost(static_cast<Derived*>(this)))); }

    constexpr ConstIterator begin() const { return ConstIterator(leftMost(static_cast<const Derived*>(this))); }
    constexpr ConstIterator end() const
    {
        return std::next(ConstIterator(rightMost(static_cast<const Derived*>(this))));
    }

protected:
    std::unique_ptr<Derived> left_{nullptr};
    std::unique_ptr<Derived> right_{nullptr};
    Derived*                 parent_{nullptr};

private:
    // TODO: move Impl classes to its own namespace
    template <class T>
    static constexpr T* leftMostImpl(T* const current)
    {
        CORRADE_INTERNAL_ASSERT(current != nullptr);
        T* n = current;
        while (n->left_ != nullptr)
        {
            n = n->left_.get();
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }
    static constexpr Derived*       leftMost(Derived* const current) { return leftMostImpl(current); }
    static constexpr const Derived* leftMost(const Derived* current) { return leftMostImpl(current); }

    template <class T>
    static constexpr T* rightMostImpl(T* const current)
    {
        CORRADE_INTERNAL_ASSERT(current != nullptr);
        T* n = current;
        while (n->right_ != nullptr)
        {
            n = n->right_.get();
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }
    static constexpr Derived*       rightMost(Derived* const current) { return rightMostImpl(current); }
    static constexpr const Derived* rightMost(const Derived* current) { return rightMostImpl(current); }

    static constexpr Derived* next(const Derived* current)
    {
        CORRADE_INTERNAL_ASSERT(current != nullptr);
        if (current->right_ != nullptr)
        {
            return leftMost(current->right_.get());
        }

        Derived* n = current->parent_;
        while (n != nullptr && current == n->right_.get())
        {
            current = n;
            n       = n->parent_;
        }

        return n;
    }
};

#endif // CONTAINERS_BINARYTREE_H
