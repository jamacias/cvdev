#include <Magnum/ImGuiIntegration/Context.hpp>

using namespace Magnum;
class ImagePreview
{
private:
    bool fitToWindow_{false};

public:
    explicit ImagePreview();

    void draw();
};
