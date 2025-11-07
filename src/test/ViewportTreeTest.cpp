#include <algorithm>
#include <string>
#include <Corrade/TestSuite/Tester.h>
#include "../viewports/ViewportTree.h"
#include <Corrade/Utility/Debug.h>

using namespace Corrade;


namespace Test { namespace {

struct ViewportTreeTest : Corrade::TestSuite::Tester
{
    explicit ViewportTreeTest();

    void ViewportCoordinates();
    void Partition();
};

ViewportTreeTest::ViewportTreeTest()
{
    addTests({&ViewportTreeTest::ViewportCoordinates});
    addTests({&ViewportTreeTest::Partition});
}


void ViewportTreeTest::ViewportCoordinates()
{
    ViewportNode viewport;
    viewport.setWindowSize(Vector2i{800, 600});
    CORRADE_COMPARE(viewport.getCoordinates(),
                    Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // Absolute values
    viewport.setCoordinates(Range2Di(Vector2i(0, 0), Vector2i(800, 600)));
    CORRADE_COMPARE(viewport.getCoordinates(),
                    Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // Relative values
    viewport.setRelativeCoordinates(Range2D(Vector2(0.0, 0.0), Vector2(1.0, 1.0)));
    CORRADE_COMPARE(viewport.getCoordinates(),
                    Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // The window size changes and so should the viewport size
    CORRADE_COMPARE(viewport.setWindowSize({300, 300}).getCoordinates(),
                    Range2Di(Vector2i(0, 0), Vector2i(300,300)));

    // Setting a relative or absolute viewport should lead to the same result
    CORRADE_COMPARE(viewport.setWindowSize(Vector2i{800, 600}).setRelativeCoordinates(Range2D(Vector2(0.4, 0.2), Vector2(0.6, 0.9))).getCoordinates(),
                    viewport.setWindowSize(Vector2i{800, 600}).setCoordinates(Range2Di(Vector2i(320, 120), Vector2i(480, 540))).getCoordinates());
}

void ViewportTreeTest::Partition()
{
    const Vector2i windowSize(1000, 800);
    ViewportTree tree(windowSize);

    // o-----------------------+
    // |           |           |
    // |    1      |     2     |
    // |           |           |
    // +-----------------------+
    tree.divide({100, 100}, ViewportTree::PartitionDirection::VERTICAL);
    CORRADE_COMPARE(tree.findActiveViewport({50, 50})->getCoordinates(),  // 1
                    Range2Di({}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({750, 50})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));
    
    // o-----------------------+
    // |     3     |           |
    // |-----------|     2     |
    // |     1     |           |
    // +-----------------------+
    tree.divide({}, ViewportTree::PartitionDirection::HORIZONTAL);
    CORRADE_COMPARE(tree.findActiveViewport(windowSize / 4)->getCoordinates(),  // 3
                    Range2Di({}, windowSize / 2));
    CORRADE_COMPARE(tree.findActiveViewport({windowSize.x() / 4, 3 * windowSize.y() / 4})->getCoordinates(), // 1
                    Range2Di({0, windowSize.y() / 2}, {windowSize.x() / 2, windowSize.y()}));
    CORRADE_COMPARE(tree.findActiveViewport({3 * windowSize.x() / 4, windowSize.y() / 2})->getCoordinates(), // 2
                    Range2Di({windowSize.x() / 2, 0}, windowSize));
}

} // Test
} // namespace

CORRADE_TEST_MAIN(Test::ViewportTreeTest)