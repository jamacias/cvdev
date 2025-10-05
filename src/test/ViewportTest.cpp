#include <algorithm>
#include <string>
#include <Corrade/TestSuite/Tester.h>
#include "../viewports/AbstractViewport.h"
#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/Debug.h>

using namespace Corrade;


namespace Test { namespace {

struct ViewportTest : Corrade::TestSuite::Tester
{
    explicit ViewportTest();

    void Size();
    // void Iteration();
};

ViewportTest::ViewportTest()
{
    addTests({&ViewportTest::Size});
    // addTests({&ViewporTest::Iteration});
}


class DummyViewport : public AbstractViewport
{
public:
    virtual void handlePointerPressEvent(Platform::Application::PointerEvent &) override
    {
        Utility::Debug{} << "handlePointerPressEvent";
    }
    virtual void handlePointerReleaseEvent(Platform::Application::PointerEvent &) override
    {
        Utility::Debug{} << "handlePointerReleaseEvent";
    }
    virtual void handlePointerMoveEvent(Platform::Application::PointerMoveEvent &) override
    {
        Utility::Debug{} << "handlePointerMoveEvent";
    }
    virtual void handleScrollEvent(Platform::Application::ScrollEvent &) override
    {
        Utility::Debug{} << "handleScrollEvent";
    }
    virtual void draw(SceneGraph::DrawableGroup3D &) override
    {
        Utility::Debug{} << "draw";
    }
};

void ViewportTest::Size()
{
    DummyViewport viewport;
    viewport.setWindowSize(Vector2i{800, 600});

    viewport.setViewport(Range2Di(Vector2i(0, 0), Vector2i(800, 600)));
    CORRADE_COMPARE(viewport.getViewport(), Range2Di(Vector2i(0, 0), Vector2i(800, 600)));

    viewport.setRelativeViewport(Range2D(Vector2(0.0, 0.0), Vector2(1.0, 1.0)));
    CORRADE_COMPARE(viewport.getViewport(), Range2Di(Vector2i(0, 0), Vector2i(800, 600)));
}

} // Test
} // namespace

CORRADE_TEST_MAIN(Test::ViewportTest)