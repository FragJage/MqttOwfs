#include "TestMqttOwfs.h"

using namespace std;

TestMqttOwfs::TestMqttOwfs() : TestClass("MqttOwfs", this)
{
	addTest("Start", &TestMqttOwfs::Start);
	addTest("DeviceRefresh", &TestMqttOwfs::DeviceRefresh);
	addTest("DeviceSet", &TestMqttOwfs::DeviceSet);
	addTest("Commands", &TestMqttOwfs::Commands);
	addTest("Stop", &TestMqttOwfs::Stop);
	addTest("CoverageConfig", &TestMqttOwfs::CoverageConfig);

	mqttClient.SetMessageCallback(this);
	mqttClient.Open();
	mqttClient.Subscribe("owfs/#");
}

TestMqttOwfs::~TestMqttOwfs()
{
	mqttClient.Close();
}

void TestMqttOwfs::ThreadStart(MqttOwfs* pMqttDev)
{
	char exeName[] = "test";
	char confArg[] = "--configfile";
	char confName[] = "MqttOwfs.conf";
	char levelArg[] = "--loglevel";
	char levelName[] = "2";
	char destArg[] = "--logdestination";
	char destName[] = "cout";
	char* argv[7];

	argv[0] = exeName;
	argv[1] = confArg;
	argv[2] = confName;
	argv[3] = levelArg;
	argv[4] = levelName;
	argv[5] = destArg;
	argv[6] = destName;
	pMqttDev->ServiceStart(7, argv);
}

void TestMqttOwfs::on_message(const string& topic, const string& message)
{
	m_Messages[topic] = message;
}

void TestMqttOwfs::waitMsg()
{
	size_t nb = 0;

	for(int i=0; i<10; i++)
	{
		mqttClient.Loop(110);
		if(nb != m_Messages.size())
		{
			nb = m_Messages.size();
			i--;
		}
	}
}

bool TestMqttOwfs::Start()
{
	thread integrationTest(ThreadStart, &mqttOwfs);
	integrationTest.detach();
	waitMsg();

	map<string, string>::iterator it;
	it = m_Messages.find("owfs/10.2DCF462904B4");
	assert(m_Messages.end() != it);
	it = m_Messages.find("owfs/05.78D868A7FF3F");
	assert(m_Messages.end() != it);
	it = m_Messages.find("owfs/29.2BF1FCD97A96");
	assert(m_Messages.end() != it);

	m_Messages.clear();

	return true;
}

bool TestMqttOwfs::DeviceRefresh()
{
	mqttClient.Publish("owfs/command/05.78D868A7FF3F", "REQUEST");
	waitMsg();

	map<string, string>::iterator it;
	it = m_Messages.find("owfs/05.78D868A7FF3F");
	assert(m_Messages.end() != it);

	m_Messages.clear();

	return true;
}

bool TestMqttOwfs::DeviceSet()
{

	mqttClient.Publish("owfs/command/29.2BF1FCD97A96", "255");
	waitMsg();

	map<string, string>::iterator it;
	it = m_Messages.find("owfs/29.2BF1FCD97A96");
	assert(m_Messages.end() != it);

	m_Messages.clear();

	return true;
}

bool TestMqttOwfs::Commands()
{
	map<string, string>::iterator it;

	mqttClient.Publish("owfs/command", "REQUEST");
	waitMsg();
	it = m_Messages.find("owfs/05.78D868A7FF3F");
	assert(m_Messages.end() != it);
	m_Messages.clear();

	mqttClient.Publish("owfs/command", "REFRESH_DEVICES");
	waitMsg();
	assert(0 < m_Messages.size());
	m_Messages.clear();

	mqttClient.Publish("owfs/command", "REFRESH_VALUES");
	waitMsg();
	assert(0 < m_Messages.size());
	m_Messages.clear();

	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	waitMsg();
	Plateforms::delay(505);
	waitMsg();
	assert(1 == m_Messages.size());
	m_Messages.clear();

	return true;
}

bool TestMqttOwfs::Stop()
{
	mqttOwfs.ServicePause(true);
	Plateforms::delay(550);
	mqttOwfs.ServicePause(false);
	mqttOwfs.ServiceStop();
	Plateforms::delay(600);

	return true;
}

void TestMqttOwfs::ThreadConf(MqttOwfs* pMqttDev)
{
	char exeName[] = "config";
	char* argv[1];

	argv[0] = exeName;
	pMqttDev->ServiceStart(1, argv);
}

bool TestMqttOwfs::CoverageConfig()
{
    mqttOwfs.SetConfigfile("./test/data/MqttOwfs1.conf");
	thread integrationTest1(ThreadConf, &mqttOwfs);
	integrationTest1.detach();
	waitMsg();
	mqttOwfs.ServiceStop();
	Plateforms::delay(510);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs2.conf");
	thread integrationTest2(ThreadConf, &mqttOwfs);
	integrationTest2.detach();
	waitMsg();
	mqttOwfs.ServiceStop();
	Plateforms::delay(510);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs3.conf");
	thread integrationTest3(ThreadConf, &mqttOwfs);
	integrationTest3.detach();
	waitMsg();
	mqttOwfs.ServiceStop();
	Plateforms::delay(510);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs4.conf");
	thread integrationTest4(ThreadConf, &mqttOwfs);
	integrationTest4.detach();
	waitMsg();
	mqttOwfs.ServiceStop();
	Plateforms::delay(510);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs5.conf");
	thread integrationTest5(ThreadConf, &mqttOwfs);
	integrationTest5.detach();
	waitMsg();
	mqttOwfs.ServiceStop();
	Plateforms::delay(510);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs6.conf");
	thread integrationTest6(ThreadConf, &mqttOwfs);
	integrationTest6.detach();
	waitMsg();
	mqttOwfs.ServiceStop();
	Plateforms::delay(510);
    return true;
}
