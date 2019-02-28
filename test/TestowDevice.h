#include <cassert>
#include "Plateforms.h"
#include "UnitTest.h"
#include "../src/owDevice.h"

using namespace std;

class TestowDevice : public TestClass<TestowDevice>
{
public:
    TestowDevice();
    ~TestowDevice();

	bool TestConstructor();
	bool TestCopyConstructor();
	bool TestMoveAssignment();
    bool TestDisplayName();
    bool TestRound();
    bool TestValue();
    bool TestCachedBool();
    bool TestRefresh();
};
