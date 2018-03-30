#include "TestowDevice.h"

using namespace std;

TestowDevice::TestowDevice() : TestClass("owDevice", this)
{
	addTest("DisplayName", &TestowDevice::TestDisplayName);
	addTest("Round", &TestowDevice::TestRound);
	addTest("Value", &TestowDevice::TestValue);
}

TestowDevice::~TestowDevice()
{
}

bool TestowDevice::TestDisplayName()
{
    owDevice myOw("MyDisplayName", "", 0);
	string displayName = myOw.GetDisplayName();
    assert("MyDisplayName"== displayName);

    return true;
}

bool TestowDevice::TestRound()
{
    owDevice myOw("MyDisplayName", "", 2);
	int round = myOw.GetRound();
    assert(2== round);
    return true;
}

bool TestowDevice::TestValue()
{
	string value;

    owDevice myOw("MyDisplayName", "", 2, "5");
	value = myOw.GetValue();
    assert("5"== value);
    myOw.SetValue("ZZ");
	value = myOw.GetValue();
	assert("ZZ"== value);
    return true;
}
