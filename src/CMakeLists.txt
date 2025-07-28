find_package(Magnum REQUIRED
    GL
    SceneGraph
    GlfwApplication)
find_package(MagnumIntegration REQUIRED
    ImGui)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(OBJECTS_LIST
    objects/Camera.cpp
    objects/Grid.cpp)

set(PANELS_LIST
    panels/3DView.cpp
    panels/ImagePreview.cpp)

set(VIEWPORTS_LIST
    viewports/ViewportManager.cpp)


add_subdirectory(test)

add_executable(Application Application.cpp
                           ${OBJECTS_LIST}
                           ${PANELS_LIST}
                           ${VIEWPORTS_LIST})
target_link_libraries(Application PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::SceneGraph
    Magnum::Primitives
    MagnumIntegration::ImGui)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Application)
