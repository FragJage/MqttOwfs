#include <cassert>
#include "UnitTest/UnitTest.h"
#include "../src/owDevice.h"

using namespace std;

class TestowDevice : public TestClass<TestowDevice>
{
public:
    TestowDevice();
    ~TestowDevice();

    bool TestDisplayName();
    bool TestRound();
    bool TestValue();
};
