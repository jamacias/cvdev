#include <Corrade/TestSuite/Tester.h>
#include "../structures/BinaryTree.h"
#include <Corrade/Containers/Array.h>


namespace {

struct BinaryTreeTest : Corrade::TestSuite::Tester
{
    explicit BinaryTreeTest();

    void NextNode();

    void IsLeaf();

    void HelloBenchmark();
};

BinaryTreeTest::BinaryTreeTest()
{
    addTests({&BinaryTreeTest::NextNode});
    addTests({&BinaryTreeTest::IsLeaf});

    addBenchmarks({&BinaryTreeTest::HelloBenchmark}, 100);
}


void BinaryTreeTest::NextNode()
{
    using NodeType = Node<int>;

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
    // Change the iterationSequence if, the test tree has changed!
    const auto iterationSequence = Containers::array<int>({3, 1, 4, 0, 7, 5, 8, 2, 6});
    auto n_base = new NodeType(0, 
        new NodeType(1, 
            new NodeType(3), new NodeType(4)), new NodeType(2, 
                                                 new NodeType(5,
                                                   new NodeType(7), new NodeType(8)), new NodeType(6)));

    CORRADE_VERIFY(n_base->isRoot());
    CORRADE_VERIFY(n_base->root()->isRoot());

    NodeType* current = n_base->leftMost();
    Debug{} << "Start: " << current->data;
    std::size_t counter = 0;
    while (current != nullptr)
    {
        CORRADE_VERIFY(current->data == iterationSequence[counter]);
        CORRADE_VERIFY(current->root()->data == n_base->data); // the root of all nodes in the tree should be the same
        Debug{} << "Current: " << current->data;
        current = current->next();
        ++counter;
    }
}

// TODO: addChildren() and test for construction with and without children

void BinaryTreeTest::IsLeaf()
{
    using NodeType = Node<int>;

    auto base = new NodeType(0);
    CORRADE_VERIFY(base->isLeaf() && base->isRoot());

    // Children not yet linked to base, therefore they are also roots and leaves too
    auto leftChild = new NodeType(1);
    CORRADE_VERIFY(leftChild->isLeaf() && leftChild->isRoot());
    auto rightChild = new NodeType(2);
    CORRADE_VERIFY(rightChild->isLeaf() && rightChild->isRoot());

    // Base is recreated with the children
    base = new NodeType(0, leftChild, rightChild);
    CORRADE_VERIFY(!base->isLeaf() && base->isRoot());
    CORRADE_VERIFY(leftChild->isLeaf() && !leftChild->isRoot());
    CORRADE_VERIFY(rightChild->isLeaf() && !rightChild->isRoot());
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