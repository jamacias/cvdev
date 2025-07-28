#ifndef STRUCTURES_BINARYTREE_H
#define STRUCTURES_BINARYTREE_H

// #include <Magnum/SceneGraph/Object.h>
// #include <Magnum/SceneGraph/MatrixTransformation2D.h>
// #include <Corrade/Containers/LinkedList.h>
#include <Corrade/Utility/Debug.h>
#include <memory>
#include <thread>

// using namespace Magnum;
using namespace Corrade;

// class Node : public SceneGraph::Object<SceneGraph::MatrixTransformation2D>
// {
//     explicit Node();
// };

// template <class T>
// class Node : public std::enable_shared_from_this<Node<T>>
// {
// public:
//     using NodeType = Node<T>;
    
//     explicit Node(const T &data, std::shared_ptr<NodeType> left = nullptr, std::shared_ptr<NodeType> right = nullptr)
//     : data_(data)
//     , left_(std::move(left))
//     , right_(std::move(right))
//     {
//         CORRADE_INTERNAL_ASSERT((left_ == nullptr && right_ == nullptr) ||
//                                 (left_ != nullptr && right_ != nullptr));

//         if (left_ != nullptr)
//         {
//             left_->parent_  = this->weak_from_this();
//             // Utility::Debug{} << (left_->parent_.expired() ? "nullptr" : "valid");
//         }
//         if (right_ != nullptr) right_->parent_ = this->weak_from_this();
//         print();
//         // Utility::Debug{} << "Node: " << data_ << " left_ is " << (left_ == nullptr ? "nullptr" : "valid") << " while right_ is " << (right_ == nullptr ? "nullptr" : "valid") << " and parent_ is " << (parent_.expired() ? "nullptr" : "valid");
//         // if (left_ != nullptr)  left_->parent_  = this;
//         // if (right_ != nullptr) right_->parent_ = this;
//     }

//     Node() = default;
//     ~Node() = default;
//     Node(const NodeType &other) = delete;
//     Node(NodeType &&other) noexcept = default;
//     NodeType &operator=(const NodeType &other) = delete;
//     NodeType &operator=(NodeType &&other) noexcept = default;

//     static std::shared_ptr<Node> create(const T &data, std::shared_ptr<NodeType> left = nullptr, std::shared_ptr<NodeType> right = nullptr)
//     {
//         auto p = std::make_shared<Node>();

//         return p;
//     }

//     constexpr bool isRoot() const noexcept { return parent_.expired(); }
//     constexpr bool isLeaf() const noexcept { return !left_ && !right_; }

//     constexpr std::shared_ptr<NodeType> parent() const noexcept { return parent_.lock(); }
//     constexpr std::shared_ptr<NodeType> left() const noexcept { return left_; }
//     constexpr std::shared_ptr<NodeType> right() const noexcept { return right_; }
//     constexpr T& data() noexcept { return data_; }

//     constexpr void print() const
//     {
//         // Utility::Debug{} << (left_ == nullptr ? "nullptr" : std::to_string(left_->data_).c_str());
//         Utility::Debug{} << "---\nNode:" << data_ << "; left_ is" << (left_ == nullptr ? "nullptr" : std::to_string(left_->data_).c_str()) << "while right_ is" << (right_ == nullptr ? "nullptr" : std::to_string(right_->data_).c_str()) << "and parent_ is" << (parent_.expired() ? "nullptr" : std::to_string(parent_.lock()->data_).c_str()) << "\n---";
//     }

//     // constexpr const NodeType& data() const noexcept { return data_; }

//     // constexpr void addLeftChild(cons)

//     std::shared_ptr<NodeType> root()
//     {
//         std::shared_ptr<NodeType> root = this->shared_from_this();
//         while (!parent_.expired())
//         {
//             root = parent_.lock();
//         }

//         CORRADE_INTERNAL_ASSERT(root->isRoot());

//         return root;
//     }

//     std::shared_ptr<NodeType> first()
//     {
//         std::shared_ptr<NodeType> first = root();

//         return leftMost();
//     }

//     // std::shared_ptr<NodeType> next()
//     // {
//     //     std::shared_ptr<NodeType> next = nullptr;

//     //     if (right_ == nullptr) // therefore the left is also nullptr
//     //     // if (isLeaf())
//     //     {
//     //         // Traverse upwards
//     //         CORRADE_INTERNAL_ASSERT(right_ == left_ && left_ == nullptr);

//     //         next = this->shared_from_this();
//     //         // while (next->parent_.lock() == nullptr && next == next->parent_->right_)
//     //         while (next->parent_.expired() && next == next->parent_.lock()->right_)
//     //         {
//     //             next = next->parent_.lock();
//     //         }
//     //         next = next->parent_.lock();
//     //     }
//     //     else
//     //     {
//     //         // Find the leftmost branch
//     //         next = right_;
//     //         while (next->left_ != nullptr)
//     //         {
//     //             next = left_;
//     //         }
//     //     }

//     //     return next;
//     // }

//     constexpr std::shared_ptr<NodeType> next()
//     {
//         if (right_ != nullptr)
//         {
//             return leftMost();
//         }

//         std::shared_ptr<NodeType> n = this->shared_from_this();
//         while (!n->parent_.expired() && n == n->parent_.lock()->right_)
//         {
//             n = n->parent_.lock();
//         }

//         return n->parent_.lock();
//     }

// private:
//     T data_;
//     std::weak_ptr<NodeType> parent_;
//     std::shared_ptr<NodeType> left_   {nullptr};
//     std::shared_ptr<NodeType> right_  {nullptr};

//     constexpr std::shared_ptr<NodeType> leftMost()
//     {
//         std::shared_ptr<NodeType> n = this->shared_from_this();
//         while (n->left_ != nullptr)
//         {
//             n = n->left_;
//         }

//         return n;
//     }
// };

template <class T>
class Node
{
public:
    using NodeType = Node<T>;
    
    // TODO: use a constructor with a bool to create children or better to pass in the children?
    Node(const T &data, NodeType *left = nullptr, NodeType *right = nullptr)
    {
        this->data = data;
        this->left_ = left;
        this->right_ = right;
        if (left != nullptr) left_->parent_ = this;
        if (right != nullptr) right_->parent_ = this;

        printPtrs();

        // TODO: check that the parent-child relationships are correct
    }

    ~Node() = default;
    Node(const NodeType &other) = delete;
    Node(NodeType &&other) noexcept = default;
    NodeType &operator=(const NodeType &other) = delete;
    NodeType &operator=(NodeType &&other) noexcept = default;

    constexpr bool isRoot() const noexcept { return parent_ == nullptr; }

    NodeType* root()
    {
        NodeType* root = this;
        while (parent_ != nullptr)
        {
            root = parent_;
        }

        CORRADE_INTERNAL_ASSERT(root->isRoot());

        return root;
    }

    // TODO: use a pointer since this may be doing something wrong.
    constexpr NodeType* leftMost()
    {
        NodeType* n = this;
        while (n->left_ != nullptr)
        {
            n = n->left_;
        }

        return n;
    }

    constexpr NodeType* next()
    {
        // Utility::Debug{} << "next called";
        // printPtrs();
        if (right_ != nullptr)
        {
            // Utility::Debug{} << "Right leftmost";
            return right_->leftMost();
        }
        else
        {
            NodeType* n = this;
            // n->printPtrs();
            while (n->parent_ != nullptr && n == n->parent_->right_)
            {
                n = n->parent_;
                // Utility::Debug{} << "n->parent_->right ptrs => ";
                // n->parent_->right_->printPtrs();
                // n = n->parent_->right_;
                // n->printPtrs();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

            // Utility::Debug{} << "n = " << n->parent_->data;

            // n->printPtrs();

            return n->parent_;
        }

        // ------

        // NodeType* next;

        // if (right_ == nullptr)
        // {
        //     // Node has no right child
        //     next = this;
        //     while (next->parent_ != nullptr && next == next->parent_->right_)
        //     {
        //         next = next->parent_;
        //     }
        //     next = next->parent_;
        // } 
        // else
        // {
        //     // Find the leftmost node in the right subtree
        //     next = right_;
        //     while (next->left_ != nullptr)
        //     {
        //         next = next->left_;
        //     }
        // }

        // return next;
    }

    T data;

    void printPtrs()
    {
        Utility::Debug{} << "this = " << this << "; &left_ = " << left_ << "; &right = " << right_ << "; &parent_ = " << parent_ << "; data = " << data;
    }

private:
    NodeType* left_{nullptr};
    NodeType* right_{nullptr};
    NodeType* parent_{nullptr};
};

// template<class NodeType>
// class BinaryTree
// {
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

// public:
//     explicit BinaryTree(const NodeType &root)
//     : root_(root)
//     {

//     }

//     const Node &first()
//     {
//         return root_.leftMost();
//     }

//     const Node &last()
//     {

//     }

//     // void find(const std::function<bool()>)
//     // {
//     //     // This can actually be done with begin() and end() and std::find_if
//     // }

//     void addChildren(const NodeType &left, const NodeType& right)
//     {

//     }
// };

#endif // STRUCTURES_BINARYTREE_H