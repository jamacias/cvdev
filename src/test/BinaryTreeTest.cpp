#include <algorithm>
#include <string>
#include <Corrade/TestSuite/Tester.h>
#include "../structures/BinaryTree.h"
#include <Corrade/Containers/Array.h>


namespace {

struct BinaryTreeTest : Corrade::TestSuite::Tester
{
    explicit BinaryTreeTest();

    void Size();

    void Iterator();

    void HelloBenchmark();
};

BinaryTreeTest::BinaryTreeTest()
{
    addTests({&BinaryTreeTest::Size});
    addTests({&BinaryTreeTest::Iterator});

    addBenchmarks({&BinaryTreeTest::HelloBenchmark}, 100);
}

class TreeNode : public BinaryTree::Node
{
public:
    int data;
    explicit TreeNode(int data)
    : data(data) {}

    void printPtrs() const
    {
        Utility::Debug{} << "this = " << this << "; &left = " << left_ << "; &right = " << right_ << "; &parent_ = " << parent_ << "; data = " << data;
    
        /*
             parent
               |
             data
             /   \
          left  right
        */
        const auto printNodeIfValid = [](Node* const n)->const char*
            {
                return (n ? std::to_string(static_cast<TreeNode*>(n)->data).c_str() : "null");
            };
        Utility::Debug{} << "  " << printNodeIfValid(parent_)
                         << "\n   |\n  "
                         << data
                         << "\n  / \\\n"
                         << printNodeIfValid(left_) << " " << printNodeIfValid(right_)
                         ;
    }
};

const auto printTree =[](const BinaryTree &tree)->void
{
    std::for_each(tree.begin(), tree.end(), [](const auto& node)
        {
            static_cast<const TreeNode&>(node).printPtrs();
        });
};

constexpr auto checkSequence = [](const BinaryTree &tree, const Containers::ArrayView<BinaryTree::Type> &sequence)->void
{
    std::size_t index = 0;
    std::for_each(tree.begin(), tree.end(), [&](const auto& node)
        {
            CORRADE_VERIFY(static_cast<const TreeNode&>(node).data == sequence[index]);
            ++index;
        });
};

constexpr auto contains = [](const BinaryTree& tree, const TreeNode& treeNode)->bool
{
    for (const auto& node : tree)
    {
        if (&node == &treeNode)
            return true;
    }
    return false;
};

constexpr auto countNodes = [](const BinaryTree& tree)->std::size_t
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

    TreeNode root(0);

    BinaryTree tree(&root);
    CORRADE_VERIFY(tree.size() == 1);
    CORRADE_VERIFY(tree.size() == countNodes(tree));
    TreeNode node1(1);
    TreeNode node2(2);
    tree.insert(&root, &node1, &node2);
    CORRADE_VERIFY(tree.size() == 3);
    CORRADE_VERIFY(tree.size() == countNodes(tree));
    TreeNode node3(3);
    TreeNode node4(4);
    tree.insert(&node1, &node3, &node4);
    CORRADE_VERIFY(tree.size() == 5);
    CORRADE_VERIFY(tree.size() == countNodes(tree));
    TreeNode node5(5);
    TreeNode node6(6);
    tree.insert(&node2, &node5, &node6);
    CORRADE_VERIFY(tree.size() == 7);
    CORRADE_VERIFY(tree.size() == countNodes(tree));
    TreeNode node7(7);
    TreeNode node8(8);
    tree.insert(&node5, &node7, &node8);
    CORRADE_VERIFY(tree.size() == 9);
    CORRADE_VERIFY(tree.size() == countNodes(tree));

    // Delete one leaf, should delete the sibling too
    tree.remove(&node8);
    CORRADE_VERIFY(tree.size() == 7);
    CORRADE_VERIFY(!contains(tree, node8));

    // Delete a node that is not a leaf, should delete the sibling and all its children
    tree.remove(&node1);
    CORRADE_VERIFY(tree.size() == 1);
    CORRADE_VERIFY(!contains(tree, node1));
}

void BinaryTreeTest::Iterator()
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

    TreeNode root(0);

    BinaryTree tree(&root);
    TreeNode node1(1);
    TreeNode node2(2);
    tree.insert(&root, &node1, &node2);
    TreeNode node3(3);
    TreeNode node4(4);
    tree.insert(&node1, &node3, &node4);
    TreeNode node5(5);
    TreeNode node6(6);
    tree.insert(&node2, &node5, &node6);
    TreeNode node7(7);
    TreeNode node8(8);
    tree.insert(&node5, &node7, &node8);

    checkSequence(tree, Containers::array({3, 1, 4, 0, 7, 5, 8, 2, 6}));
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

} // namespace

CORRADE_TEST_MAIN(BinaryTreeTest)