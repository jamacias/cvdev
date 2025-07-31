#ifndef STRUCTURES_BINARYTREE_H
#define STRUCTURES_BINARYTREE_H

#include <Corrade/Utility/Debug.h>
#include <memory>
#include <string>

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
private:
    struct Node; // forward declaration
public:
    using Type = int;
    explicit BinaryTree(const Type &rootData)
    {
        root_ = std::make_shared<Node>(rootData);
        size_++;

        /*
        The example tree is represented as follows:
              0
            /   \
           1     2
          / \   / \
         3   4 5   6
              / \
             7   8
        */
        addChildren(root_, 1, 2);
        addChildren(root_->left, 3, 4);
        addChildren(root_->right, 5, 6);
        addChildren(root_->right->left, 7, 8);
    }

    // const Node &first()
    // {
    //     return root_.leftMost();
    // }
    // const Node &last() { }

    // const Node* begin() { return nullptr; }
    // const Node* end() { return nullptr; }

    void forEach()
    {
        std::shared_ptr<Node> current = leftMost(root_);
        while (current != nullptr)
        {
            Utility::Debug{} << "current->data: " << current->data << "; owners: " << current.use_count();
            current = next(current);
        }
    }

    std::shared_ptr<Node> find(const Type &data)
    {
        std::shared_ptr<Node> current = leftMost(root_);
        while (current != nullptr)
        {
            // Utility::Debug{} << "current->data: " << current->data;
            if (current->data == data)
                return current;

            current = next(current);
        }

        return nullptr;
    }

    void insert(const Type &parent, const Type &left, const Type &right)
    {
        if (const auto parentPtr = find(parent))
        {
            CORRADE_INTERNAL_ASSERT(parentPtr);
            CORRADE_INTERNAL_ASSERT(parentPtr->data == parent);
            CORRADE_ASSERT(parentPtr->isLeaf(), "BinaryTree::insert(): the parent cannot already have children", );
            addChildren(parentPtr, left, right);
        }
    }

    void remove(const Type &parent)
    {
        std::shared_ptr<Node> parentPtr{nullptr};
        if (parentPtr = find(parent))
        {
            CORRADE_INTERNAL_ASSERT(parentPtr);
            CORRADE_INTERNAL_ASSERT(parentPtr->data == parent);
            std::shared_ptr<Node> nextPtr {nullptr};
            do
            {
                std::shared_ptr<Node> current = leftMost(parentPtr)->parent.lock();
                removeChildren(current);
                nextPtr = next(current);
            } while (nextPtr != nullptr && nextPtr->isLeaf() && nextPtr == parentPtr);

            removeChildren(parentPtr->parent.lock());
            size_ -= 2;
        }
    }

    std::size_t size() const
    {
        return size_;
    }

    // void divide(const Type &value);
    // void merge(const Type &value);

private:
    struct Node
    {
        explicit Node(const Type &data) : data(data){}
        Type data;
        std::shared_ptr<Node> left{nullptr};
        std::shared_ptr<Node> right{nullptr};
        std::weak_ptr<Node> parent;

        bool isRoot() const { return parent.expired(); }
        bool isLeaf() const { return !left && !right; }
        void printPtrs()
        {
            Utility::Debug{} << "this = " << this << "; &left = " << left.get() << "; &right = " << right.get() << "; &parent_ = " << parent.lock().get() << "; data = " << data;

            /*
                 parent
                   |
                 data
                 /   \
              left  right
            */
            const auto printNodeIfValid = [](const Node* n)->const char*
                {
                    return (n ? std::to_string(n->data).c_str() : "null");
                };
            Utility::Debug{} << "  " << printNodeIfValid(parent.lock().get())
                             << "\n   |\n  "
                             << data
                             << "\n  / \\\n"
                             << printNodeIfValid(left.get()) << " " << printNodeIfValid(right.get())
                             ;
        }
    };

    std::shared_ptr<Node> root_{nullptr};
    std::size_t size_{0};

    std::shared_ptr<Node> leftMost(std::shared_ptr<Node> current)
    {
        std::shared_ptr<Node> n =  current;
        while (n->left != nullptr)
        {
            n = n->left;
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }

    std::shared_ptr<Node> next(std::shared_ptr<Node> current)
    {
        CORRADE_INTERNAL_ASSERT(current != nullptr);
        if (current->right != nullptr)
        {
            return leftMost(current->right);
        }

        std::shared_ptr<Node> n = current->parent.lock();
        while (n != nullptr && current == n->right)
        {
            current = n;
            n = n->parent.lock();
        }

        return n;
    }

    void addChildren(const std::shared_ptr<Node>& parent, const Type left, const Type right)
    {
        CORRADE_INTERNAL_ASSERT(parent);
        CORRADE_INTERNAL_ASSERT(parent->isLeaf());

        parent->left = std::make_shared<Node>(left);
        parent->left->parent = parent;
        parent->right = std::make_shared<Node>(right);
        parent->right->parent = parent;

        size_ += 2;

        parent->printPtrs();
        CORRADE_INTERNAL_ASSERT(!parent->isLeaf());
        CORRADE_INTERNAL_ASSERT(parent->left->parent.lock() == parent->right->parent.lock()); // TODO: may be done faster (https://stackoverflow.com/q/12301916)
        CORRADE_INTERNAL_ASSERT(size_ % 2);
    }

    void removeChildren(const std::shared_ptr<Node>& parent)
    {
        CORRADE_INTERNAL_ASSERT(parent);
        CORRADE_INTERNAL_ASSERT(( parent->left &&  parent->right) ||
                                (!parent->left && !parent->right));
        if (parent->left)
        {
            parent->left = nullptr;
            size_--;
        }
        if (parent->right)
        {
            parent->right = nullptr;
            size_--;
        }
        CORRADE_INTERNAL_ASSERT(size_ % 2);
    }
};

#endif // STRUCTURES_BINARYTREE_H