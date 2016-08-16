#include "TestSystem.hpp"

TestSystem::TestSystem()
{
    // Initialize a new Goldilocks testmanager.
    testmanager = new TestManager;

    /* We merely need to register the suites with the system.
     * Loading their tests is now interactively on-demand. */
    testmanager->register_suite("P-sB10", new TestSuite_FlexArray());
    testmanager->register_suite("P-sB16", new TestSuite_Pool());
    testmanager->register_suite("P-sB40", new TestSuite_Onestring());
    testmanager->register_suite("P-sB30", new TestSuite_Pawsort());
}

TestSystem::~TestSystem()
{
    delete testmanager;
}
