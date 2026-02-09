find_package(Corrade REQUIRED TestSuite)

enable_testing()

corrade_add_test(BinaryTreeTest BinaryTreeTest.cpp)
corrade_add_test(ViewportTest ViewportTest.cpp
    ../viewports/AbstractViewport.cpp
    LIBRARIES Magnum)
corrade_add_test(ViewportTreeTest ViewportTreeTest.cpp
    ../viewports/ViewportTree.cpp ../viewports/AbstractViewport.cpp
    LIBRARIES Magnum)
