#include <Corrade/TestSuite/Tester.h>


namespace {

struct HelloWorld : Corrade::TestSuite::Tester
{
    explicit HelloWorld();

    void Hello();

    void HelloBenchmark();
};

HelloWorld::HelloWorld()
{
    addTests({&HelloWorld::Hello});

    addBenchmarks({&HelloWorld::HelloBenchmark}, 100);
}

void HelloWorld::Hello()
{
    double a = 5.0;
    double b = 3.0;

    CORRADE_VERIFY(a*b == b*a);
    CORRADE_VERIFY(a/b != b/a);
}

void HelloWorld::HelloBenchmark()
{
    double a{};
    CORRADE_BENCHMARK(100)
    {
        a = 1;
    }
    CORRADE_VERIFY(a); // to avoid the benchmark loop being optimized out
}

} // namespace

CORRADE_TEST_MAIN(HelloWorld)