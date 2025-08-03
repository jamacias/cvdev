#include <Corrade/TestSuite/Tester.h>
#include "../structures/BinaryTree.h"
#include <Corrade/Containers/Array.h>


namespace {

struct BinaryTreeTest : Corrade::TestSuite::Tester
{
    explicit BinaryTreeTest();

    void NextNode();

    void IsLeaf();

    void ForEach();

    void HelloBenchmark();
};

BinaryTreeTest::BinaryTreeTest()
{
    addTests({&BinaryTreeTest::NextNode});
    addTests({&BinaryTreeTest::IsLeaf});
    addTests({&BinaryTreeTest::ForEach});

    addBenchmarks({&BinaryTreeTest::HelloBenchmark}, 100);
}


void BinaryTreeTest::NextNode()
{
    // using NodeType = Node<int>;

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
    // const auto iterationSequence = Containers::array<int>({3, 1, 4, 0, 7, 5, 8, 2, 6});
    // auto n_base = new NodeType(0, 
    //     new NodeType(1, 
    //         new NodeType(3), new NodeType(4)), new NodeType(2, 
    //                                              new NodeType(5,
    //                                                new NodeType(7), new NodeType(8)), new NodeType(6)));

    // CORRADE_VERIFY(n_base->isRoot());
    // CORRADE_VERIFY(n_base->root()->isRoot());

    // NodeType* current = n_base->leftMost();
    // Debug{} << "Start: " << current->data;
    // std::size_t counter = 0;
    // while (current != nullptr)
    // {
    //     CORRADE_VERIFY(current->data == iterationSequence[counter]);
    //     CORRADE_VERIFY(current->root()->data == n_base->data); // the root of all nodes in the tree should be the same
    //     Debug{} << "Current: " << current->data;
    //     current = current->next();
    //     ++counter;
    // }
    CORRADE_VERIFY(true);
}

// TODO: addChildren() and test for construction with and without children

void BinaryTreeTest::IsLeaf()
{
    // using NodeType = Node<int>;

    // auto base = new NodeType(0);
    // CORRADE_VERIFY(base->isLeaf() && base->isRoot());

    // // Children not yet linked to base, therefore they are also roots and leaves too
    // auto leftChild = new NodeType(1);
    // CORRADE_VERIFY(leftChild->isLeaf() && leftChild->isRoot());
    // auto rightChild = new NodeType(2);
    // CORRADE_VERIFY(rightChild->isLeaf() && rightChild->isRoot());

    // // Base is recreated with the children
    // base = new NodeType(0, leftChild, rightChild);
    // CORRADE_VERIFY(!base->isLeaf() && base->isRoot());
    // CORRADE_VERIFY(leftChild->isLeaf() && !leftChild->isRoot());
    // CORRADE_VERIFY(rightChild->isLeaf() && !rightChild->isRoot());
    CORRADE_VERIFY(true);
}

void BinaryTreeTest::ForEach()
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

    BinaryTree tree(0);
    CORRADE_VERIFY(tree.size() == 1);
    tree.insert(0, 1, 2);
    CORRADE_VERIFY(tree.size() == 3);
    tree.insert(1, 3, 4);
    CORRADE_VERIFY(tree.size() == 5);
    tree.insert(2, 5, 6);
    CORRADE_VERIFY(tree.size() == 7);
    tree.insert(5, 7, 8);
    CORRADE_VERIFY(tree.size() == 9);

    // tree.forEach([](auto& n){ Debug{} << n.data; });
    const auto checkSequence = [](const BinaryTree &tree, const Containers::ArrayView<BinaryTree::Type> &sequence)
    {
        std::size_t index = 0;
        tree.forEach([&](auto& n)
        {
            CORRADE_INTERNAL_ASSERT(n.data == sequence[index]);
            ++index;
        });
    };
    checkSequence(tree, Containers::array({3, 1, 4, 0, 7, 5, 8, 2, 6}));
    
    Debug{} << "--- Find --- ";
    CORRADE_VERIFY(tree.find(5)->data == 5);
    CORRADE_VERIFY(tree.find(1232425) == nullptr);

    // Debug{} << "--- First --- ";
    // Debug{} << tree.first().data;

    // Debug{} << "--- Last --- ";
    // Debug{} << tree.last().data;

    Debug{} << "--- Insert --- ";
    tree.insert(6, 9, 10);
    CORRADE_VERIFY(tree.size() == 11);
    checkSequence(tree, Containers::array({3, 1, 4, 0, 7, 5, 8, 2, 9, 6, 10}));

    Debug{} << "--- Remove --- ";
    tree.remove(6);
    CORRADE_VERIFY(tree.size() == 5);
    checkSequence(tree, Containers::array({3, 1, 4, 0, 2}));

    Debug{} << "--- Remove --- ";
    tree.remove(2);
    CORRADE_VERIFY(tree.size() == 1);
    checkSequence(tree, Containers::array({0}));

    // TODO: do not allow removing the root
    // Debug{} << "--- Remove --- ";
    // tree.remove(0);
    // Debug{} << "Size: " << tree.size();
    // tree.forEach([](auto& n){ Debug{} << n.data; });

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