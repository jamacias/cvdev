#ifndef STRUCTURES_BINARYTREE_H
#define STRUCTURES_BINARYTREE_H

#include <Corrade/Utility/Debug.h>
#include <memory>
#include <string>
#include <functional>

using namespace Corrade;


template <class T>
class Node
{
public:
    using Type = T;

    Node(const T &data, Node<Type> *left = nullptr, Node<Type> *right = nullptr)
    {
        this->data = data;
        this->left_ = left;
        this->right_ = right;
        if (left != nullptr) left_->parent_ = this;
        if (right != nullptr) right_->parent_ = this;
    }

    ~Node() = default;
    Node(const Node<Type> &other) = delete;
    Node(Node<Type> &&other) noexcept = default;
    Node<Type> &operator=(const Node<Type> &other) = delete;
    Node<Type> &operator=(Node<Type> &&other) noexcept = default;

    constexpr bool isRoot() const noexcept { return parent_ == nullptr; }
    constexpr bool isLeaf() const noexcept { return !left_ && !right_; }

    constexpr Node<Type>* root()
    {
        Node<Type>* root = this;
        while (root->parent_ != nullptr)
        {
            root = parent_->root();
        }

        CORRADE_INTERNAL_ASSERT(root->isRoot());

        return root;
    }

    constexpr Node<Type>* leftMost()
    {
        Node<Type>* n = this;
        while (n->left_ != nullptr)
        {
            n = n->left_;
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }

    constexpr Node<Type>* next()
    {
        if (right_ != nullptr)
        {
            return right_->leftMost();
        }
        else
        {
            Node<Type>* n = this;
            while (n->parent_ != nullptr && n == n->parent_->right_)
            {
                n = n->parent_;
            }

            return n->parent_;
        }
    }

    T data;

    void printPtrs()
    {
        Utility::Debug{} << "this = " << this << "; &left_ = " << left_ << "; &right = " << right_ << "; &parent_ = " << parent_ << "; data = " << data;
    }

private:
    Node<Type>* left_{nullptr};
    Node<Type>* right_{nullptr};
    Node<Type>* parent_{nullptr};
};

// template<class T>
class BinaryTree
{
public:
    using Type = int;
    class Node
    {
        friend BinaryTree;
    public:
        constexpr explicit Node(){}
        // Node(const Node&) = delete;
        // Node(Node&& other){};
        // Node& operator=(const Node&) = delete;
        // Node& operator=(Node&& other){};
    
        bool isRoot() const { return parent_; }
        bool isLeaf() const { return !left_ && !right_; }
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

    const Node* begin() { return leftMost(root_); }
    const Node* end() { return nullptr; }

    void forEach(const std::function<void(Node&)>& f = nullptr) const
    {
        Node* current = leftMost(root_);
        while (current != nullptr)
        {
            // Utility::Debug{} << "current->data: " << current->data << "; owners: " << current.use_count();
            if (f)
                f(*current);
            current = next(current);
        }
    }

    void insert(Node* parent, Node* left, Node* right)
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

    void remove(Node* child)
    {
        CORRADE_INTERNAL_ASSERT(child);

        Node* nextPtr {nullptr};
        do
        {
            Node* current = leftMost(child->parent_)->parent_;

            if (current->left_)
                current->left_ = nullptr;
            if (current->right_)
                current->right_ = nullptr;

            size_ -= 2;

            nextPtr = next(current);
        } while (nextPtr != nullptr && nextPtr->isLeaf() && nextPtr == child->parent_);

        if (nextPtr->left_)
                nextPtr->left_ = nullptr;
        if (nextPtr->right_)
            nextPtr->right_ = nullptr;

        size_ -= 2;
    }

    constexpr std::size_t size() const
    {
        return size_;
    }

private:
    Node* root_{nullptr};
    std::size_t size_{0};

    static Node* leftMost(Node* const current)
    {
        Node* n =  current;
        while (n->left_ != nullptr)
        {
            n = n->left_;
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }

    static Node* next(const Node* current)
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