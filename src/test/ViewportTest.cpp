#include <algorithm>
#include <string>
#include <Corrade/TestSuite/Tester.h>
#include "../viewports/AbstractViewport.h"
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Debug.h>

using namespace Corrade;

namespace Test
{
namespace
{

struct ViewportTest : Corrade::TestSuite::Tester
{
    explicit ViewportTest();

    void ViewportSize();
};

ViewportTest::ViewportTest()
{
    addTests({&ViewportTest::ViewportSize});
}

class DummyViewport : public AbstractViewport
{
public:
    virtual void handlePointerPressEvent(Platform::Application::PointerEvent&) override
    {
        Utility::Debug{} << "handlePointerPressEvent";
    }
    virtual void handlePointerReleaseEvent(Platform::Application::PointerEvent&) override
    {
        Utility::Debug{} << "handlePointerReleaseEvent";
    }
    virtual void handlePointerMoveEvent(Platform::Application::PointerMoveEvent&) override
    {
        Utility::Debug{} << "handlePointerMoveEvent";
    }
    virtual void handleScrollEvent(Platform::Application::ScrollEvent&) override
    {
        Utility::Debug{} << "handleScrollEvent";
    }
    virtual void draw(SceneGraph::DrawableGroup3D&) override { Utility::Debug{} << "draw"; }
};

void ViewportTest::ViewportSize()
{
    DummyViewport viewport;
    viewport.setWindowSize(Vector2i{800, 600});
    CORRADE_COMPARE(viewport.getViewport(), Range2Di(Vector2i(0, 0), Vector2i(0, 0)));

    // Absolute values
    viewport.setViewport(Range2Di(Vector2i(0, 0), Vector2i(800, 600)));
    CORRADE_COMPARE(viewport.getViewport(), Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // Relative values
    viewport.setRelativeViewport(Range2D(Vector2(0.0, 0.0), Vector2(1.0, 1.0)));
    CORRADE_COMPARE(viewport.getViewport(), Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    // The window size changes and so should the viewport size
    CORRADE_COMPARE(viewport.setWindowSize({300, 300}).getViewport(), Range2Di(Vector2i(0, 0), Vector2i(300, 300)));

    // Setting a relative or absolute viewport should lead to the same result
    CORRADE_COMPARE(viewport.setWindowSize(Vector2i{800, 600})
                        .setRelativeViewport(Range2D(Vector2(0.4, 0.2), Vector2(0.6, 0.9)))
                        .getViewport(),
                    viewport.setWindowSize(Vector2i{800, 600})
                        .setViewport(Range2Di(Vector2i(320, 120), Vector2i(480, 540)))
                        .getViewport());
}

} // namespace
} // namespace Test

CORRADE_TEST_MAIN(Test::ViewportTest)
