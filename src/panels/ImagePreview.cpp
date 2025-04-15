#include "ImagePreview.h"

ImagePreview::ImagePreview()
{
}

void ImagePreview::draw()
{
    const ImGuiWindowFlags windowFlags = {ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize};    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1280, 720));
    
    ImGui::Begin("Image preview", nullptr, windowFlags);

    ImGui::Text("This is a test window. Here there should be an image");

    ImGui::End();
}