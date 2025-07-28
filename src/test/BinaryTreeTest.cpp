#include <Corrade/TestSuite/Tester.h>
#include "../structures/BinaryTree.h"
#include <Corrade/Containers/Array.h>


namespace {

struct BinaryTreeTest : Corrade::TestSuite::Tester
{
    explicit BinaryTreeTest();

    void NextNode();

    void NextNode2();

    void HelloBenchmark();
};

BinaryTreeTest::BinaryTreeTest()
{
    addTests({&BinaryTreeTest::NextNode});
    addTests({&BinaryTreeTest::NextNode2});

    addBenchmarks({&BinaryTreeTest::HelloBenchmark}, 100);
}

Node<int>* CreateSampleTree()
{
    return nullptr;
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
        // Debug{} << "current->root()->data: " << current->root()->data;
        // CORRADE_VERIFY(current->root()->data == n_base->data); // the root of all nodes should be the same
        Debug{} << "Current: " << current->data;
        current = current->next();
        ++counter;
    }
}

void BinaryTreeTest::NextNode2()
{
    // using NodeType = Node<int>;

    // NodeType root(0);
    // CORRADE_VERIFY(root.leftMost().data == 0);
    
    // root.left = new NodeType();
    // root.left->data = 1;
    // Debug{} << root.leftMost().data;
    // Debug{} << root.left->data;
    // CORRADE_VERIFY(root.leftMost().data == root.left->data);
    CORRADE_VERIFY(true);
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