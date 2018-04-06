#include "TestowDevice.h"

using namespace std;

TestowDevice::TestowDevice() : TestClass("owDevice", this)
{
	addTest("CopyConstructor", &TestowDevice::TestCopyConstructor);
	addTest("DisplayName", &TestowDevice::TestDisplayName);
	addTest("Round", &TestowDevice::TestRound);
	addTest("Value", &TestowDevice::TestValue);
}

TestowDevice::~TestowDevice()
{
}

bool TestowDevice::TestCopyConstructor()
{
	owDevice myOwOne("MyDisplayName", "MyInternalName", 2, "5");
	owDevice myOwCp(myOwOne);
	owDevice myOwEq = myOwOne;

	string Name2 = myOwCp.GetDisplayName();
	string Name3 = myOwEq.GetDisplayName();
	assert("MyDisplayName" == Name2);
	assert("MyDisplayName" == Name3);

	int Rnd2 = myOwCp.GetRound();
	int Rnd3 = myOwEq.GetRound();
	assert(2 == Rnd2);
	assert(2 == Rnd3);

	string Value2 = myOwCp.GetValue();
	string Value3 = myOwEq.GetValue();
	assert("5" == Value2);
	assert("5" == Value3);

	return true;
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
