#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>

#include <Magnum/Ui/UserInterfaceGL.h>

using namespace Magnum;

class RoboCockpit : public Platform::Application
{
public:
    explicit RoboCockpit(const Arguments &arguments);

private:
    void viewportEvent(ViewportEvent &event) override;
    void drawEvent() override;
    void pointerPressEvent(PointerEvent &event) override;
    void pointerReleaseEvent(PointerEvent &event) override;
    void pointerMoveEvent(PointerMoveEvent &event) override;
    void scrollEvent(ScrollEvent &event) override;
    void keyPressEvent(KeyEvent &event) override;
    void keyReleaseEvent(KeyEvent &event) override;
    void textInputEvent(TextInputEvent &event) override;

    Ui::UserInterfaceGL ui_{NoCreate};
};
