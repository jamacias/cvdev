#include <Corrade/TestSuite/Tester.h>
#include "../structures/BinaryTree.h"


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

void BinaryTreeTest::NextNode()
{
    using NodeType = Node<int>;
    // BinaryTree bt;
    // auto node = std::make_shared<NodeType>(1);
    // auto n_base = std::make_shared<NodeType>(0, 
    //     std::make_shared<NodeType>(1, 
    //         std::make_shared<NodeType>(3), std::make_shared<NodeType>(4)), std::make_shared<NodeType>(2, 
    //                                                                           std::make_shared<NodeType>(5,
    //                                                                             std::make_shared<NodeType>(7), std::make_unique<NodeType>(8)), std::make_unique<NodeType>(6)));
    auto n_base = new NodeType(0, 
        new NodeType(1, 
            new NodeType(3), new NodeType(4)), new NodeType(2, 
                                                 new NodeType(5,
                                                   new NodeType(7), new NodeType(8)), new NodeType(6)));
    // auto n_base = new NodeType(0, new NodeType(1), new NodeType(2));
    // n_base->left_->printPtrs();
    // n_base->leftMost()->printPtrs();
    // auto n_base = std::make_shared<NodeType>(0, std::move(node), std::make_unique<NodeType>(2));

    // NodeType n_base(0, new NodeType(1), new NodeType(2, new NodeType(3), new NodeType(4)));

    // auto n_base = std::make_shared<NodeType>(0);

    // BinaryTree tree(1);

    CORRADE_VERIFY(n_base->isRoot());
    CORRADE_VERIFY(n_base->root()->isRoot());

    // Debug{} << "Left most: " << n_base->leftMost().data;
    // Debug{} << "Left most next: " << n_base->leftMost().next().data;
    // Debug{} << "Left most next next: " << n_base->leftMost().next().next().data;
    // Debug{} << "Left most next next next: " << n_base->leftMost().next().next().next().data;

    NodeType* current = n_base->leftMost();
    // NodeType* current = n_base;
    Debug{} << "Start: " << current->data;
    while (current != nullptr)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        Debug{} << "Current: " << current->data;
        current = current->next();
    }
    

    // Debug{} << "First: " << n_base->first()->data();

    // Debug{} << "Next: " << n_base->first()->next()->data();
    // Debug{} << "Next next: " << n_base->first()->next()->next()->data();

    double a = 5.0;
    double b = 3.0;

    CORRADE_VERIFY(a*b == b*a);
    CORRADE_VERIFY(a/b != b/a);
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