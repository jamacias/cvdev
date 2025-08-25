#ifndef STRUCTURES_BINARYTREE_H
#define STRUCTURES_BINARYTREE_H

using namespace Corrade;

#include <Corrade/Utility/Debug.h>

template<class T>
class BinaryTree
{
public:
    constexpr explicit BinaryTree(T* root)
    : root_(root)
    {
        size_++;
    }

    // constexpr const Node& first() const
    // {
    //     return first();
    // }

    // Node& first()
    // {
    //     return *leftMost(root_);
    // }
    
    // constexpr const Node& last() const
    // {
    //     return last();
    // }

    // Node& last()
    // {
    //     return *rightMost(root_);
    // }

    class Iterator
    {
    public:
        constexpr Iterator(T* node) : node_(node){}

        constexpr T& operator*() const
        {
            return *node_;
        }

        constexpr bool operator!=(const Iterator& other) const
        {
            return node_ != other.node_;
        }

        Iterator& operator++()
        {
            node_ = next(node_);
            return *this;
        }

    private:
        T* node_;
    };

    Iterator begin() { return Iterator(leftMost(root_)); }
    Iterator end() { return Iterator(nullptr); }


    class ConstIterator
    {
    public:
        constexpr ConstIterator(const T* node) : node_(node){}

        constexpr const T& operator*() const
        {
            return *node_;
        }

        constexpr bool operator!=(const ConstIterator& other) const
        {
            return node_ != other.node_;
        }

        ConstIterator& operator++()
        {
            node_ = next(node_);
            return *this;
        }

    private:
        const T* node_;
    };

    constexpr ConstIterator begin() const { return ConstIterator(leftMost(root_)); }
    constexpr ConstIterator end() const { return ConstIterator(nullptr); }

    constexpr void insert(T* parent, T* left, T* right)
    {
        CORRADE_INTERNAL_ASSERT(parent && left && right);
        CORRADE_ASSERT(parent->isLeaf(), "BinaryTree::insert(): the parent cannot already have children", );
        // At the bottom => append if it is a leaf

        parent->left_ = left;
        parent->right_ = right;

        left->parent_ = parent;
        right->parent_ = parent;

        // TODO:
        // At the top => move the root node
        // At the middle => move nodes and insert in the middle

        size_ += 2;
    }

    constexpr void remove(T* node)
    {
        if (!node) return;

        T* nextPtr {nullptr};
        do
        {
            T* current = leftMost(node->parent_)->parent_;

            if (current->left_)
                current->left_ = nullptr;
            if (current->right_)
                current->right_ = nullptr;

            size_ -= 2;

            nextPtr = next(current);
        } while (nextPtr != nullptr && nextPtr->isLeaf() && nextPtr != node->parent_);

        remove(node->parent_->right_);

        if (!node->isLeaf())
        {
            size_ -= 2;
        }
    }

    constexpr std::size_t size() const
    {
        return size_;
    }

private:
    T* root_{nullptr};
    std::size_t size_{0};

    static constexpr T* leftMost(T* const current)
    {
        T* n =  current;
        while (n->left_ != nullptr)
        {
            n = n->left_;
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }

    static constexpr T* next(const T* current)
    {
        CORRADE_INTERNAL_ASSERT(current != nullptr);
        if (current->right_ != nullptr)
        {
            return leftMost(current->right_);
        }

        T* n = current->parent_;
        while (n != nullptr && current == n->right_)
        {
            current = n;
            n = n->parent_;
        }

        return n;
    }
};


template<class Derived>
class Node
{
    friend BinaryTree<Derived>;
public:
    constexpr explicit Node()
    : left_(nullptr)
    , right_(nullptr)
    , parent_(nullptr)
    {}
    Node(const Node<Derived>&) = delete;
    Node<Derived>(Node<Derived>&& other) = delete; //{};
    Node<Derived>& operator=(const Node<Derived>&) = delete;
    Node<Derived>& operator=(Node<Derived>&& other) = delete;//{};
    virtual ~Node() = default; //{ Utility::Debug{} << "Destroy"; };
    
    constexpr bool isRoot() const { return parent_; }
    constexpr bool isLeaf() const { return !left_ && !right_; }
protected:
    Derived* left_{nullptr};
    Derived* right_{nullptr};
    Derived* parent_{nullptr};
};

#endif // STRUCTURES_BINARYTREE_H