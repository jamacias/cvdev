#include <algorithm>
#include <string>
#include <Corrade/TestSuite/Tester.h>
#include "../containers/BinaryTree.h"
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Containers/Pointer.h>

using namespace Corrade;


namespace Test { namespace {

struct BinaryTreeTest : Corrade::TestSuite::Tester
{
    explicit BinaryTreeTest();

    void Size();
    void NodeIteration();
    void Iteration();
    void MoveTree();
    void LeafAndRoot();
    void CutNode();
    void InsertNode();
    void Siblings();

    void HelloBenchmark();
};

BinaryTreeTest::BinaryTreeTest()
{
    addTests({&BinaryTreeTest::Size});
    addTests({&BinaryTreeTest::Iteration});
    addTests({&BinaryTreeTest::NodeIteration});
    addTests({&BinaryTreeTest::MoveTree});
    addTests({&BinaryTreeTest::LeafAndRoot});
    addTests({&BinaryTreeTest::CutNode});
    addTests({&BinaryTreeTest::InsertNode});
    addTests({&BinaryTreeTest::Siblings});

    addBenchmarks({&BinaryTreeTest::HelloBenchmark}, 100);
}


class TreeNode : public Node<TreeNode>
{
public:
    using Type = int;
    Type data;
    explicit TreeNode(int data)
    : data(data) {}

    constexpr bool operator==(const Type value) const { return data == value; }
    constexpr bool operator==(const TreeNode &node) const { return data == node.data /*&& this == &node*/; }

    // Expose left and right so that we can better test when the trees are imbalanced and cannot be iterated
    TreeNode const* left() const { return left_.get(); }
    TreeNode const* right() const { return right_.get(); }

    void printPtrs() const
    {
        Utility::Debug{} << "this = " << this << "; &left = " << left_.get() << "; &right = " << right_.get() << "; &parent_ = " << parent_ << "; data = " << data;
    
        /*
             parent
               |
             data
             /   \
          left  right
        */
        const auto printNodeIfValid = [](TreeNode* const n)->const char*
            {
                return (n ? std::to_string(n->data).c_str() : "null");
            };
        Utility::Debug{} << "  " << printNodeIfValid(parent_)
                         << "\n   |\n  "
                         << data
                         << "\n  / \\\n"
                         << printNodeIfValid(left_.get()) << " " << printNodeIfValid(right_.get())
                         ;
    }
};

using Tree = BinaryTree<TreeNode>;

const auto printTree =[](const Tree &tree)->void
{
    std::for_each(tree.begin(), tree.end(), [](const auto& node)
        {
            node.printPtrs();
        });
};

constexpr auto checkSequence = [](const Tree &tree, const Containers::ArrayView<TreeNode::Type> &sequence)->bool
{
    std::size_t index = 0;
    bool ok = true;
    std::for_each(tree.begin(), tree.end(), [&](const auto& node)
        {
            if (node.data != sequence[index])
            {
                ok = false;
            }
            ++index;
        });

    return ok;
};

// Helper function similar to std::equal that also checks the size.
template<class T, class N>
constexpr auto equal = [](const T &tree, const N &node)->bool
{
    if (std::distance(tree.begin(), tree.end()) != std::distance(node.begin(), node.end()))
    {
        return false;
    }
    
    auto treeBeginning = tree.begin();
    auto nodeBeginning = node.begin();
    for (; treeBeginning != tree.end(); ++treeBeginning, ++nodeBeginning)
    {
        if (!(*treeBeginning == *nodeBeginning))
        {
            return false;
        }
    }

    return true;
};

constexpr auto contains = [](const Tree& tree, const TreeNode::Type& value)->bool
{
    const auto it = std::find(tree.begin(), tree.end(), value);

    return it != tree.end();
};

constexpr auto countNodes = [](const Tree& tree)->std::size_t
{
    std::size_t size = 0;
    for ([[maybe_unused]] const auto& node : tree)
    {
        ++size;
    }
    return size;
};


void BinaryTreeTest::Size()
{
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

    auto root = std::make_unique<TreeNode>(0);

    Tree tree(std::move(root));
    CORRADE_COMPARE(tree.size(), 1);
    CORRADE_COMPARE(tree.size(), countNodes(tree));

    tree.insert(std::find(tree.begin(), tree.end(), 0),
                std::make_unique<TreeNode>(1),
                std::make_unique<TreeNode>(2));
    CORRADE_COMPARE(tree.size(), 3);
    CORRADE_COMPARE(tree.size(), countNodes(tree));

    tree.insert(std::find(tree.begin(), tree.end(), 1),
                std::make_unique<TreeNode>(3),
                std::make_unique<TreeNode>(4));
    CORRADE_COMPARE(tree.size(), 5);
    CORRADE_COMPARE(tree.size(), countNodes(tree));

    tree.insert(std::find(tree.begin(), tree.end(), 2),
                std::make_unique<TreeNode>(5),
                std::make_unique<TreeNode>(6));
    CORRADE_COMPARE(tree.size(), 7);
    CORRADE_COMPARE(tree.size(), countNodes(tree));

    tree.insert(std::find(tree.begin(), tree.end(), 5),
                std::make_unique<TreeNode>(7),
                std::make_unique<TreeNode>(8));
    CORRADE_COMPARE(tree.size(), 9);
    CORRADE_COMPARE(tree.size(), countNodes(tree));

    // Delete one leaf, should delete the sibling too
    tree.remove(std::find(tree.begin(), tree.end(), 8));
    CORRADE_COMPARE(tree.size(), countNodes(tree));
    CORRADE_COMPARE(tree.size(), 7);
    CORRADE_VERIFY(!contains(tree, 8));
    CORRADE_VERIFY(!contains(tree, 7));
    CORRADE_VERIFY(checkSequence(tree, Containers::array({3, 1, 4, 0, 5, 2, 6})));

    // Delete a node that is not a leaf, should delete the sibling and all its children
    tree.remove(std::find(tree.begin(), tree.end(), 1));
    CORRADE_COMPARE(tree.size(), countNodes(tree));
    CORRADE_COMPARE(tree.size(), 1);
    CORRADE_VERIFY(!contains(tree, 1));
    CORRADE_VERIFY(!contains(tree, 2));
    CORRADE_VERIFY(!contains(tree, 3));
    CORRADE_VERIFY(!contains(tree, 4));
    CORRADE_VERIFY(!contains(tree, 5));
    CORRADE_VERIFY(!contains(tree, 6));
    CORRADE_VERIFY(checkSequence(tree, Containers::array({0})));
}

void BinaryTreeTest::NodeIteration()
{
    /*
          0
    */
    Tree tree(std::make_unique<TreeNode>(0));
    const auto zero = std::find(tree.begin(), tree.end(), 0);
    // TODO: for some reason std::equal does not work here
    CORRADE_VERIFY(equal<Tree, TreeNode>(tree, *zero));
    CORRADE_VERIFY(equal<TreeNode, Containers::Array<TreeNode::Type>>(*zero, Containers::array({0})));

    /*
          0
        /   \
       1     2
    */
    tree.insert(zero,
                std::make_unique<TreeNode>(1),
                std::make_unique<TreeNode>(2));
    CORRADE_VERIFY(equal<Tree, TreeNode>(tree, *zero));
    const auto one = std::find(tree.begin(), tree.end(), 1);
    CORRADE_VERIFY(equal<Tree, TreeNode>(tree, *zero));
    CORRADE_VERIFY(equal<TreeNode, Containers::Array<TreeNode::Type>>(*zero, Containers::array({1, 0, 2})));
    CORRADE_VERIFY(equal<TreeNode, Containers::Array<TreeNode::Type>>(*one, Containers::array({1})));

    /*
          0
        /   \
       1     2
      / \
     3   4
    */
    tree.insert(one,
                std::make_unique<TreeNode>(3),
                std::make_unique<TreeNode>(4));
    CORRADE_VERIFY(equal<Tree, TreeNode>(tree, *zero));
    
    // Iterating from the node should not traverse the whole tree
    CORRADE_VERIFY(equal<TreeNode, Containers::Array<TreeNode::Type>>(*one, Containers::array({3, 1, 4})));
    const auto three = std::find(tree.begin(), tree.end(), 3);
    CORRADE_VERIFY(equal<TreeNode, Containers::Array<TreeNode::Type>>(*three, Containers::array({3})));
}

void BinaryTreeTest::Iteration()
{
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

    Tree tree(std::make_unique<TreeNode>(0));
    CORRADE_VERIFY(checkSequence(tree, Containers::array({0})));

    tree.insert(std::find(tree.begin(), tree.end(), 0),
                std::make_unique<TreeNode>(1),
                std::make_unique<TreeNode>(2));
    CORRADE_VERIFY(checkSequence(tree, Containers::array({1, 0, 2})));

    tree.insert(std::find(tree.begin(), tree.end(), 1),
                std::make_unique<TreeNode>(3),
                std::make_unique<TreeNode>(4));
    CORRADE_VERIFY(checkSequence(tree, Containers::array({3, 1, 4, 0, 2})));

    tree.insert(std::find(tree.begin(), tree.end(), 2),
                std::make_unique<TreeNode>(5),
                std::make_unique<TreeNode>(6));
    CORRADE_VERIFY(checkSequence(tree, Containers::array({3, 1, 4, 0, 5, 2, 6})));

    tree.insert(std::find(tree.begin(), tree.end(), 5),
                std::make_unique<TreeNode>(7),
                std::make_unique<TreeNode>(8));
    CORRADE_VERIFY(checkSequence(tree, Containers::array({3, 1, 4, 0, 7, 5, 8, 2, 6})));
}

void BinaryTreeTest::MoveTree()
{
    Tree originalTree(std::make_unique<TreeNode>(0));
    originalTree.insert(std::find(originalTree.begin(), originalTree.end(), 0),
                        std::make_unique<TreeNode>(1),
                        std::make_unique<TreeNode>(2));
    CORRADE_COMPARE(originalTree.size(), 3);
    CORRADE_VERIFY(checkSequence(originalTree, Containers::array({1, 0, 2})));

    Tree movedTree(std::move(originalTree));
    CORRADE_COMPARE(movedTree.size(), 3);
    CORRADE_VERIFY(checkSequence(movedTree, Containers::array({1, 0, 2})));
    CORRADE_COMPARE(originalTree.size(), 0);
    CORRADE_VERIFY(checkSequence(originalTree, Containers::Array<TreeNode::Type>()));
}

void BinaryTreeTest::LeafAndRoot()
{
    Tree tree(std::make_unique<TreeNode>(0));
    const auto zero = std::find(tree.begin(), tree.end(), 0);
    CORRADE_COMPARE(zero->isLeaf(), true);
    CORRADE_COMPARE(zero->isRoot(), true);

    tree.insert(zero,
                std::make_unique<TreeNode>(1),
                std::make_unique<TreeNode>(2));
    const auto one = std::find(tree.begin(), tree.end(), 1);
    const auto two = std::find(tree.begin(), tree.end(), 2);
    CORRADE_COMPARE(zero->isLeaf(), false);
    CORRADE_COMPARE(zero->isRoot(), true);
    CORRADE_COMPARE(one->isLeaf(), true);
    CORRADE_COMPARE(one->isRoot(), false);
    CORRADE_COMPARE(one->isLeaf(), two->isLeaf());
    CORRADE_COMPARE(one->isRoot(), two->isRoot());
}

void BinaryTreeTest::CutNode()
{
    /*
    Start with:
          0
        /   \
       1     2
      / \ 
     3   4
    */
    Tree tree(std::make_unique<TreeNode>(0));
    tree.insert(std::find(tree.begin(), tree.end(), 0),
                        std::make_unique<TreeNode>(1),
                        std::make_unique<TreeNode>(2));
    CORRADE_COMPARE(tree.size(), 3);
    CORRADE_VERIFY(checkSequence(tree, Containers::array({1, 0, 2})));

    tree.insert(std::find(tree.begin(), tree.end(), 1),
                std::make_unique<TreeNode>(3),
                std::make_unique<TreeNode>(4));
    CORRADE_COMPARE(tree.size(), 5);
    CORRADE_VERIFY(checkSequence(tree, Containers::array({3, 1, 4, 0, 2})));

    {
        // Cut the root
        const auto nodeToCut = std::find(tree.begin(), tree.end(), 0);
        const auto nodeToCutRef = nodeToCut.get();
        auto cutNode = tree.cut(nodeToCut);
        CORRADE_COMPARE(cutNode.get(), nodeToCutRef);
        CORRADE_COMPARE(cutNode->isRoot(), true);
        CORRADE_COMPARE(cutNode->isLeaf(), false);

        // Rebuild tree to continue the tests
        tree = Tree(std::move(cutNode));
    }

    {
        // Cut an intermediate node
        const auto nodeToCut = std::find(tree.begin(), tree.end(), 1);
        const auto nodeToCutRef = nodeToCut.get();
        auto cutNode = tree.cut(nodeToCut);
        CORRADE_COMPARE(cutNode.get(), nodeToCutRef);
        CORRADE_COMPARE(cutNode->isRoot(), true);
        CORRADE_COMPARE(cutNode->isLeaf(), false);

        // Rebuild tree to continue the tests, this time it looks like:
        /*
          1
         / \
        3   4
        */
        tree = Tree(std::move(cutNode));
    }

    {
        // Cut a leaf node
        const auto nodeToCut = std::find(tree.begin(), tree.end(), 4);
        const auto nodeToCutRef = nodeToCut.get();
        auto cutNode = tree.cut(nodeToCut);
        CORRADE_COMPARE(cutNode.get(), nodeToCutRef);
        CORRADE_COMPARE(cutNode->isRoot(), true);
        CORRADE_COMPARE(cutNode->isLeaf(), true);
    }
}

void BinaryTreeTest::InsertNode()
{
    /*
    Start with:
          0
        /   \
       1     2
      / \ 
     3   4
    */
    Tree tree(std::make_unique<TreeNode>(0));
    const auto zero = std::find(tree.begin(), tree.end(), 0);
    tree.insert(zero,
                nullptr,
                std::make_unique<TreeNode>(2));
    CORRADE_COMPARE(tree.size(), 2);
    tree.insert(zero,
                std::make_unique<TreeNode>(1),
                nullptr);
    CORRADE_COMPARE(tree.size(), 3);
    CORRADE_VERIFY(checkSequence(tree, Containers::array({1, 0, 2})));
}

void BinaryTreeTest::Siblings()
{
    /*
          0
        /   \
       1     2
    */
    Tree tree(std::make_unique<TreeNode>(0));
    const auto zero = std::find(tree.begin(), tree.end(), 0);
    tree.insert(zero,
                std::make_unique<TreeNode>(1),
                std::make_unique<TreeNode>(2));
    const auto one = std::find(tree.begin(), tree.end(), 1);
    CORRADE_COMPARE(one->sibling()->data, 2);
    const auto two = std::find(tree.begin(), tree.end(), 2);
    CORRADE_COMPARE(two->sibling()->data, 1);

    /*
          0
        /   \
       1     2
        \
         3
    */
    // Insert only one node. Null siblings are allowed
    tree.insert(one,
                nullptr,
                std::make_unique<TreeNode>(3));
    CORRADE_COMPARE(one->right()->sibling(), nullptr);
}

void BinaryTreeTest::HelloBenchmark()
{
    double a{};
    CORRADE_BENCHMARK(100)
    {
        a = 1;
    }
    CORRADE_VERIFY(a); // to avoid the benchmark loop being optimized out
}

} // Test
} // namespace

CORRADE_TEST_MAIN(Test::BinaryTreeTest)