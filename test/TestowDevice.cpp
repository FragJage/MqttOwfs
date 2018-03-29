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
    assert("MyDisplayName"==myOw.GetDisplayName());

    return true;
}

bool TestowDevice::TestRound()
{
    owDevice myOw("MyDisplayName", "", 2);
    assert(2==myOw.GetRound());
    return true;
}

bool TestowDevice::TestValue()
{
    owDevice myOw("MyDisplayName", "", 2, "5");
    assert("5"==myOw.GetValue());
    myOw.SetValue("ZZ");
    assert("ZZ"==myOw.GetValue());
    return true;
}
