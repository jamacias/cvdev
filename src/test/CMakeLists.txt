find_package(Corrade REQUIRED TestSuite)
enable_testing()
corrade_add_test(HelloWorldTest HelloWorldTest.cpp)
corrade_add_test(BinaryTreeTest BinaryTreeTest.cpp)