#ifndef STRUCTURES_BINARYTREE_H
#define STRUCTURES_BINARYTREE_H

using namespace Corrade;

// template<class T>
class BinaryTree
{
public:
    using Type = int;
    // template<class Derived>
    class Node
    {
        friend BinaryTree;
    public:
        constexpr explicit Node()
        : left_(nullptr)
        , right_(nullptr)
        , parent_(nullptr)
        {}
        Node(const Node&) = delete;
        Node(Node&& other) = delete; //{};
        Node& operator=(const Node&) = delete;
        Node& operator=(Node&& other) = delete;//{};
        virtual ~Node(){};
    
        constexpr bool isRoot() const { return parent_; }
        constexpr bool isLeaf() const { return !left_ && !right_; }
    protected:
        Node* left_{nullptr};
        Node* right_{nullptr};
        Node* parent_{nullptr};

    };
public:
    constexpr explicit BinaryTree(Node* root)
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
        constexpr Iterator(Node* node) : node_(node){}

        constexpr Node& operator*() const
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
        Node* node_;
    };

    Iterator begin() { return Iterator(leftMost(root_)); }
    Iterator end() { return Iterator(nullptr); }


    class ConstIterator
    {
    public:
        constexpr ConstIterator(const Node* node) : node_(node){}

        constexpr const Node& operator*() const
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
        const Node* node_;
    };

    constexpr ConstIterator begin() const { return ConstIterator(leftMost(root_)); }
    constexpr ConstIterator end() const { return ConstIterator(nullptr); }

    constexpr void insert(Node* parent, Node* left, Node* right)
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

    constexpr void remove(Node* node)
    {
        // CORRADE_INTERNAL_ASSERT(node);

        if (!node) return;

        Node* nextPtr {nullptr};
        do
        {
            Node* current = leftMost(node->parent_)->parent_;

            if (current->left_)
                current->left_ = nullptr;
            if (current->right_)
                current->right_ = nullptr;

            size_ -= 2;

            nextPtr = next(current);
        } while (nextPtr != nullptr && nextPtr->isLeaf() && nextPtr != node->parent_);

        remove(node->parent_->right_);

        size_ -= 2;
    }

    constexpr std::size_t size() const
    {
        return size_;
    }

private:
    Node* root_{nullptr};
    std::size_t size_{0};

    static constexpr Node* leftMost(Node* const current)
    {
        Node* n =  current;
        while (n->left_ != nullptr)
        {
            n = n->left_;
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }

    static constexpr Node* next(const Node* current)
    {
        CORRADE_INTERNAL_ASSERT(current != nullptr);
        if (current->right_ != nullptr)
        {
            return leftMost(current->right_);
        }

        Node* n = current->parent_;
        while (n != nullptr && current == n->right_)
        {
            current = n;
            n = n->parent_;
        }

        return n;
    }
};

#endif // STRUCTURES_BINARYTREE_H