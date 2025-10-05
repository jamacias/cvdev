add_subdirectory(corrade EXCLUDE_FROM_ALL)

set(MAGNUM_WITH_GLFWAPPLICATION ON CACHE BOOL "" FORCE)
add_subdirectory(magnum EXCLUDE_FROM_ALL)

set(MAGNUM_WITH_UI ON CACHE BOOL "" FORCE)
add_subdirectory(magnum-extras EXCLUDE_FROM_ALL)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(MAGNUM_WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(magnum-integration EXCLUDE_FROM_ALL)