#include "../viewports/ViewportTree.h"

#include <Corrade/TestSuite/Tester.h>
#include <Corrade/Utility/Debug.h>

using namespace Corrade;

namespace Test
{
namespace
{

struct ViewportTreeTest : Corrade::TestSuite::Tester
{
    explicit ViewportTreeTest();

    void ViewportCoordinates();
    void BasicPartition();
    void Partition();
    void PaneAdjustment();
};

ViewportTreeTest::ViewportTreeTest()
{
    addTests({&ViewportTreeTest::ViewportCoordinates});
    addTests({&ViewportTreeTest::BasicPartition});
    addTests({&ViewportTreeTest::Partition});
    addTests({&ViewportTreeTest::PaneAdjustment});
}

const auto print = [](const ViewportTree<ViewportNode>& tree) -> void
{
    for (const auto& v : tree)
    {
        if (v.isVisible())
            Utility::Debug{} << v.getCoordinates();
    }
};

void ViewportTreeTest::ViewportCoordinates()
{
    ViewportNode viewport;
    viewport.setWindowSize(Vector2i{800, 600});
    CORRADE_COMPARE(viewport.getCoordinates(), Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // Absolute values
    viewport.setCoordinates(Range2Di(Vector2i(0, 0), Vector2i(800, 600)));
    CORRADE_COMPARE(viewport.getCoordinates(), Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // Relative values
    viewport.setRelativeCoordinates(Range2D(Vector2(0.0, 0.0), Vector2(1.0, 1.0)));
    CORRADE_COMPARE(viewport.getCoordinates(), Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // The window size changes and so should the viewport size
    CORRADE_COMPARE(viewport.setWindowSize({300, 300}).getCoordinates(), Range2Di(Vector2i(0, 0), Vector2i(300, 300)));

    // Setting a relative or absolute viewport should lead to the same result
    CORRADE_COMPARE(viewport.setWindowSize(Vector2i{800, 600})
                        .setRelativeCoordinates(Range2D(Vector2(0.4, 0.2), Vector2(0.6, 0.9)))
                        .getCoordinates(),
                    viewport.setWindowSize(Vector2i{800, 600})
                        .setCoordinates(Range2Di(Vector2i(320, 120), Vector2i(480, 540)))
                        .getCoordinates());
}

void ViewportTreeTest::BasicPartition()
{
    const Vector2i windowSize(1000, 800);
    // o-----------------------+
    // |                       |
    // |           1           |
    // |                       |
    // +-----------------------+
    ViewportTree<ViewportNode> tree(windowSize);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, windowSize));

    // o-----------------------+
    // |           |           |
    // |     1     |     2     |
    // |           |           |
    // +-----------------------+
    tree.divide({100, 100}, PartitionDirection::VERTICAL);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({750, 50})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));

    // o-----------------------+
    // |                       |
    // |           2           |
    // |                       |
    // +-----------------------+
    tree.collapse({50, 50});
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 2
                    Range2Di({}, windowSize));

    // o-----------------------+
    // |           2           |
    // |-----------------------|
    // |           3           |
    // +-----------------------+
    tree.divide({50, 50}, PartitionDirection::HORIZONTAL);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 2
                    Range2Di({}, {windowSize.x(), windowSize.y() / 2}));
    CORRADE_COMPARE(tree.findActiveViewport({50, 750})->getCoordinates(), // 3
                    Range2Di({0, windowSize.y() / 2}, windowSize));

    // o-----------------------+
    // |                       |
    // |           3           |
    // |                       |
    // +-----------------------+
    tree.collapse({50, 50});
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 3
                    Range2Di({}, windowSize));
}

void ViewportTreeTest::Partition()
{
    CORRADE_COMPARE(1, 1);
    const Vector2i             windowSize(1000, 800);
    ViewportTree<ViewportNode> tree(windowSize);

    // o-----------------------+
    // |           |           |
    // |     1     |     2     |
    // |           |           |
    // +-----------------------+
    tree.divide({100, 100}, PartitionDirection::VERTICAL);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({750, 50})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));

    // o-----------------------+
    // |     3     |           |
    // |-----------|     2     |
    // |     1     |           |
    // +-----------------------+
    tree.divide({}, PartitionDirection::HORIZONTAL);
    CORRADE_COMPARE(tree.findActiveViewport(windowSize / 4)->getCoordinates(), // 3
                    Range2Di({}, windowSize / 2));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 4, 3 * windowSize.y() / 4})->getCoordinates(), // 1
                    Range2Di({0, windowSize.y() / 2}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));

    // o-----------------------+
    // |     3     |           |
    // |-----------|     2     |
    // |  1  |  4  |           |
    // +-----------------------+
    tree.divide({windowSize.x() / 4, 3 * windowSize.y() / 4}, PartitionDirection::VERTICAL);
    CORRADE_COMPARE(tree.findActiveViewport(windowSize / 4)->getCoordinates(), // 3
                    Range2Di({}, windowSize / 2));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 8, 3 * windowSize.y() / 4})->getCoordinates(), // 1
                    Range2Di({0, windowSize.y() / 2}, {windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 8, 3 * windowSize.y() / 4})->getCoordinates(), // 4
                    Range2Di({windowSize.x() / 4, windowSize.y() / 2}, {windowSize.x() / 2, windowSize.y()}));

    // o-----------------------+
    // |     3     |     |     |
    // |-----------|  2  |  5  |
    // |  1  |  4  |     |     |
    // +-----------------------+
    tree.divide({3 * windowSize.x() / 4, windowSize.y() / 2}, PartitionDirection::VERTICAL);
    CORRADE_COMPARE(tree.findActiveViewport(windowSize / 4)->getCoordinates(), // 3
                    Range2Di({}, windowSize / 2));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 8, 3 * windowSize.y() / 4})->getCoordinates(), // 1
                    Range2Di({0, windowSize.y() / 2}, {windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({5 * windowSize.x() / 8, windowSize.y() / 2})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, {3 * windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 8, 3 * windowSize.y() / 4})->getCoordinates(), // 4
                    Range2Di({windowSize.x() / 4, windowSize.y() / 2}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({7 * windowSize.x() / 8, windowSize.y() / 2})->getCoordinates(), // 5
                    Range2Di({6 * windowSize.x() / 8, 0}, windowSize));

    // o-----------------------+
    // |     3     |           |
    // |-----------|     2     |
    // |  1  |  4  |           |
    // +-----------------------+
    tree.collapse({7 * windowSize.x() / 8, windowSize.y() / 2});
    CORRADE_COMPARE(tree.findActiveViewport(windowSize / 4)->getCoordinates(), // 3
                    Range2Di({}, windowSize / 2));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 8, 3 * windowSize.y() / 4})->getCoordinates(), // 1
                    Range2Di({0, windowSize.y() / 2}, {windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 8, 3 * windowSize.y() / 4})->getCoordinates(), // 4
                    Range2Di({windowSize.x() / 4, windowSize.y() / 2}, {windowSize.x() / 2, windowSize.y()}));

    // o-----------------------+
    // |     |     |           |
    // |  1  |  4  |     2     |
    // |     |     |           |
    // +-----------------------+
    tree.collapse(windowSize / 4);
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 8, windowSize.y() / 2})->getCoordinates(), // 1
                    Range2Di({0, 0}, {windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 8, 3 * windowSize.y() / 4})->getCoordinates(), // 4
                    Range2Di({windowSize.x() / 4, 0}, {windowSize.x() / 2, windowSize.y()}));

    // o-----------------------+
    // |           |           |
    // |     1     |     4     |
    // |           |           |
    // +-----------------------+
    tree.collapse(3 * windowSize / 4);
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 4
                    Range2Di({windowSize.x() / 2, 0}, windowSize));

    // o-----------------------+
    // |                       |
    // |           4           |
    // |                       |
    // +-----------------------+
    tree.collapse(3 * windowSize / 4);
    CORRADE_COMPARE(tree.findActiveViewport(windowSize / 2)->getCoordinates(), // 4
                    Range2Di({0, 0}, windowSize));
}

void ViewportTreeTest::PaneAdjustment()
{
    const Vector2i windowSize(1000, 800);
    // o-----------------------+
    // |                       |
    // |           1           |
    // |                       |
    // +-----------------------+
    ViewportTree<ViewportNode> tree(windowSize);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, windowSize));

    // o-----------------------+
    // |           |           |
    // |     1     |     2     |
    // |           |           |
    // +-----------------------+
    tree.divide({100, 100}, PartitionDirection::VERTICAL);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({750, 50})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));

    // o-----------------------+
    // |               |       |
    // |     1         |   2   |
    // |               |       |
    // +-----------------------+
    tree.adjust(windowSize / 2, windowSize.x() / 4);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {3 * windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({750, 50})->getCoordinates(), // 2
                    Range2Di({3 * windowSize.x() / 4, 0}, windowSize));

    // o-----------------------+
    // |       |               |
    // |   1   |       2       |
    // |       |               |
    // +-----------------------+
    tree.adjust(windowSize / 2, -windowSize.x() / 2);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({750, 50})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 4, 0}, windowSize));

    // o-----------------------+
    // |       |       2       |
    // |   1   |---------------|
    // |       |       3       |
    // +-----------------------+
    tree.divide(windowSize / 2, PartitionDirection::HORIZONTAL);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 2, windowSize.y() / 4})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 4, 0}, {windowSize.x(), windowSize.y() / 2}));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 2, 3 * windowSize.y() / 4})->getCoordinates(), // 3
                    Range2Di({windowSize.x() / 4, windowSize.y() / 2}, {windowSize.x(), windowSize.y()}));

    // o-----------------------+
    // |       |---------------|
    // |   1   |       3       |
    // |       |               |
    // +-----------------------+
    tree.adjust({windowSize.x() / 2, windowSize.y() / 4}, -windowSize.y() / 4);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 4, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 2, windowSize.y() / 8})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 4, 0}, {windowSize.x(), windowSize.y() / 4}));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 2, 3 * windowSize.y() / 4})->getCoordinates(), // 3
                    Range2Di({windowSize.x() / 4, windowSize.y() / 4}, {windowSize.x(), windowSize.y()}));

    // o-----------------------+
    // |           |-----------|
    // |     1     |     3     |
    // |           |           |
    // +-----------------------+
    tree.adjust({windowSize.x() / 4, windowSize.y() / 2}, windowSize.x() / 4);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 8})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, {windowSize.x(), windowSize.y() / 4}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, 3 * windowSize.y() / 4})->getCoordinates(), // 3
                    Range2Di({windowSize.x() / 2, windowSize.y() / 4}, {windowSize.x(), windowSize.y()}));

    // o-----------------------+
    // |           |           |
    // |     1     |     3     |
    // |           |           |
    // +-----------------------+
    tree.collapse({3 * windowSize.x() / 4, windowSize.y() / 16});
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 3
                    Range2Di({windowSize.x() / 2, 0}, {windowSize.x(), windowSize.y()}));

    // o-----------------------+
    // |           |     2     |
    // |     1     |-----------|
    // |           |     3     |
    // +-----------------------+
    tree.divide({3 * windowSize.x() / 4, windowSize.y() / 2}, PartitionDirection::HORIZONTAL);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(), // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 4})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, {windowSize.x(), windowSize.y() / 2}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, 3 * windowSize.y() / 4})->getCoordinates(), // 3
                    Range2Di({windowSize.x() / 2, windowSize.y() / 2}, {windowSize.x(), windowSize.y()}));

    // o-----------------------+
    // |           2           |
    // |-----------------------|
    // |           3           |
    // +-----------------------+
    tree.collapse({windowSize.x() / 4, windowSize.y() / 2});
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 2, windowSize.y() / 4})->getCoordinates(), // 2
                    Range2Di({0, 0}, {windowSize.x(), windowSize.y() / 2}));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 2, 3 * windowSize.y() / 4})->getCoordinates(), // 3
                    Range2Di({0, windowSize.y() / 2}, {windowSize.x(), windowSize.y()}));

    // o-----------------------+
    // |                       |
    // |           3           |
    // |                       |
    // +-----------------------+
    tree.collapse({windowSize.x() / 4, windowSize.y() / 4});
    CORRADE_COMPARE(tree.findActiveViewport(windowSize / 2)->getCoordinates(), // 3
                    Range2Di({0, 0}, {windowSize.x(), windowSize.y()}));
}

} // namespace
} // namespace Test

CORRADE_TEST_MAIN(Test::ViewportTreeTest)
