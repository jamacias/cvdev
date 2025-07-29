#ifndef STRUCTURES_BINARYTREE_H
#define STRUCTURES_BINARYTREE_H

#include <Corrade/Utility/Debug.h>

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
// class BinaryTree
// {
// public:
//     using Type = T;
//     explicit BinaryTree(const NodeType &root)
//     : root_(root)
//     {

//     }

//     const Node &first()
//     {
//         return root_.leftMost();
//     }
//     const Node &last() { }

//     const Node* begin() { return nullptr; }
//     const Node* end() { return nullptr; }

//     // void find(const std::function<bool()>)
//     // {
//     //     // This can actually be done with begin() and end() and std::find_if
//     // }

//     void addChildren(const NodeType &left, const NodeType& right)
//     {

//     }
// private:
//     // struct Node
//     // {
//     //     NodeType data;
//     //     Node* left   {nullptr};
//     //     Node* right  {nullptr};
//     //     Node* parent {nullptr};

//     //     const Node &leftMost()
//     //     {
//     //         Node &n = *this;
//     //         while(n.left != nullptr)
//     //         {
//     //             n = n.left;
//     //         }

//     //         return n;
//     //     }

//     //     const Node &next()
//     //     {
//     //         if (right != nullptr)
//     //         {
//     //             return leftMost();
//     //         }
            
//     //         Node &n = *this;
//     //         while (n.parent != nullptr && n == n.parent->right)
//     //         {
//     //             n = n.parent;
//     //         }
            
//     //         return n.parent;
//     //     }
//     // };

//     Node root_;
// };

#endif // STRUCTURES_BINARYTREE_H