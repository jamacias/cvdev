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
private:
    struct Node; // forward declaration
public:
    using Type = int;
    constexpr explicit BinaryTree() noexcept
    : root_(nullptr)
    , size_(0)
    {}
    // constexpr explicit BinaryTree(const Type &rootData)
    // : root_(rootData)
    // , size_(1)
    // {

    // }

    // class Iterator
    // {
    // public:
    //     constexpr Iterator(Node* node) : node_(node){}

    //     constexpr Node& operator*() const
    //     {
    //         return *node_;
    //     }

    //     constexpr Node& operator++() const
    //     {
    //         next(node_);
    //     }

    // private:
    //     Node* node_;
    // };

    // const Node* begin() { return nullptr; }
    // const Node* end() { return nullptr; }

    // void forEach(const std::function<void(Node&)>& f = nullptr) //const
    // {
    //     Node* current = leftMost(&root_);
    //     while (current != nullptr)
    //     {
    //         // Utility::Debug{} << "current->data: " << current->data << "; owners: " << current.use_count();
    //         if (f)
    //             f(*current);
    //         current = next(current);
    //     }
    // }

    // Node* find(const Type &data) //const
    // {
    //     Node* current = leftMost(&root_);
    //     while (current != nullptr)
    //     {
    //         // Utility::Debug{} << "current->data: " << current->data;
    //         if (current->data == data)
    //             return current;

    //         current = next(current);
    //     }

    //     return nullptr;
    // }

    void insert(Node* left, Node* right, Node* parent)
    {
        CORRADE_INTERNAL_ASSERT(left != nullptr);
        CORRADE_INTERNAL_ASSERT(right != nullptr);
        CORRADE_INTERNAL_ASSERT(parent != nullptr);

        CORRADE_ASSERT(!left->tree || !right->tree, "BinaryTree::insert(): cannot insert a node that already belongs elsewhere", );

        left->tree = static_cast<decltype(left->tree)>(this);
        right->tree = static_cast<decltype(right->tree)>(this);

        parent->left = left;
        left->parent = parent;
        
        parent->right = right;
        right->parent = parent;
    }

    void remove(Node* parent)
    {
        CORRADE_INTERNAL_ASSERT(parent);

        child->parent
        
        // if ((parentPtr = find(parent)))
        // {
        //     CORRADE_INTERNAL_ASSERT(parentPtr);
        //     CORRADE_INTERNAL_ASSERT(parentPtr->data == parent);
        //     Node* nextPtr {nullptr};
        //     do
        //     {
        //         Node* current = leftMost(parentPtr)->parent;
        //         removeChildren(current);
        //         nextPtr = next(current);
        //     } while (nextPtr != nullptr && nextPtr->isLeaf() && nextPtr == parentPtr);

        //     removeChildren(parentPtr->parent);
        //     size_ -= 2;
        // }
    }

    constexpr std::size_t size() const
    {
        return size_;
    }

    // void divide(const Type &value);
    // void merge(const Type &value);

private:
    // friend Iterator;
    struct Node
    {
        // constexpr explicit Node(const Type &data) : data(data){}
        constexpr explicit Node(const Type &data) noexcept
        : data(data)
        , left(nullptr)
        , right(nullptr)
        , parent(nullptr)
        , tree(nullptr)
        {}
        
        ~Node()
        {
            if (left) delete left;
            if (right) delete right;
        }
        Type data;
        Node* left{nullptr};
        Node* right{nullptr};
        Node* parent{nullptr};
        BinaryTree* tree{nullptr};

        bool isRoot() const { return parent; }
        bool isLeaf() const { return !left && !right; }
        void printPtrs() const
        {
            Utility::Debug{} << "this = " << this << "; &left = " << left << "; &right = " << right << "; &parent_ = " << parent << "; data = " << data;

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
            Utility::Debug{} << "  " << printNodeIfValid(parent)
                             << "\n   |\n  "
                             << data
                             << "\n  / \\\n"
                             << printNodeIfValid(left) << " " << printNodeIfValid(right)
                             ;
        }
    };

    Node* root_;
    std::size_t size_{0};

    Node* leftMost(Node* current) const
    {
        Node* n =  current;
        while (n->left != nullptr)
        {
            n = n->left;
        }

        CORRADE_INTERNAL_ASSERT(n->isLeaf());

        return n;
    }

    Node* next(Node* current) const
    {
        CORRADE_INTERNAL_ASSERT(current != nullptr);
        if (current->right != nullptr)
        {
            return leftMost(current->right);
        }

        Node* n = current->parent;
        while (n != nullptr && current == n->right)
        {
            current = n;
            n = n->parent;
        }

        return n;
    }

    void addChildren(Node* const parent, const Type left, const Type right)
    {
        CORRADE_INTERNAL_ASSERT(parent);
        CORRADE_INTERNAL_ASSERT(parent->isLeaf());

        parent->left = new Node(left);
        parent->left->parent = parent;
        parent->right = new Node(right);
        parent->right->parent = parent;

        size_ += 2;

        parent->printPtrs();
        CORRADE_INTERNAL_ASSERT(!parent->isLeaf());
        CORRADE_INTERNAL_ASSERT(parent->left->parent == parent->right->parent); // TODO: may be done faster (https://stackoverflow.com/q/12301916)
        CORRADE_INTERNAL_ASSERT(size_ % 2);
    }

    void removeChildren(Node* const parent)
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