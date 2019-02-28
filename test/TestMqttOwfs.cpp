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
	mqttClient.Connect();
	mqttClient.Subscribe("owfs/#");
}

TestMqttOwfs::~TestMqttOwfs()
{
	mqttClient.Disconnect();
}

void TestMqttOwfs::ThreadStart(MqttOwfs* pMqttDev)
{
	char exeName[] = "test";
	char consArg[] = "-console";
	char confArg[] = "--configfile";
	char confName[] = "MqttOwfs.conf";
	char levelArg[] = "--loglevel";
	char levelName[] = "2";
	char destArg[] = "--logdestination";
	char destName[] = "cout";
	char* argv[8];

	argv[0] = exeName;
	argv[1] = consArg;
	argv[2] = confArg;
	argv[3] = confName;
	argv[4] = levelArg;
	argv[5] = levelName;
	argv[6] = destArg;
	argv[7] = destName;

	Service* pService = Service::Create("MqttOwfs", "Mqtt protocol bridge for Owfs", pMqttDev);
	pService->Start(8, argv);
}

void TestMqttOwfs::on_message(const string& topic, const string& message)
{
	m_Messages[topic] = message;
}

void TestMqttOwfs::waitMsg(size_t maxMsg, int maxTime)
{
	int time = 0;
	bool stop = false;

	while (!stop)
	{
		if (m_Messages.size() >= maxMsg) stop = true;
		if (time > maxTime) stop = true;
		time += 50;
		Plateforms::delay(50);
	}
}

bool TestMqttOwfs::Start()
{
	thread integrationTest(ThreadStart, &mqttOwfs);
	integrationTest.detach();
	waitMsg(10, 2500);

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
	waitMsg(1, 200);

	map<string, string>::iterator it;
	it = m_Messages.find("owfs/05.78D868A7FF3F");
	assert(m_Messages.end() != it);

	m_Messages.clear();

	return true;
}

bool TestMqttOwfs::DeviceSet()
{

	mqttClient.Publish("owfs/command/29.2BF1FCD97A96", "255");
	waitMsg(1, 200);

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
	waitMsg(3, 500);
	it = m_Messages.find("owfs/05.78D868A7FF3F");
	assert(m_Messages.end() != it);
	m_Messages.clear();

	mqttClient.Publish("owfs/command", "REFRESH_DEVICES");
	waitMsg(3, 500);
	assert(0 < m_Messages.size());
	m_Messages.clear();

	mqttClient.Publish("owfs/command", "REFRESH_VALUES");
	waitMsg(3, 500);
	assert(0 < m_Messages.size());
	m_Messages.clear();

	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	waitMsg(1, 200);
	Plateforms::delay(505);
	waitMsg(2, 200);
	assert(1 <= m_Messages.size());
	m_Messages.clear();

	return true;
}

bool TestMqttOwfs::Stop()
{
	Service::Get()->ChangeStatus(Service::StatusKind::PAUSE);
	Plateforms::delay(350);
	Service::Get()->ChangeStatus(Service::StatusKind::START);
	Plateforms::delay(350);
	Service::Get()->ChangeStatus(Service::StatusKind::STOP);
	Plateforms::delay(205);

	return true;
}

void TestMqttOwfs::ThreadConf(MqttOwfs* pMqttDev)
{
	char exeName[] = "config";
	char consArg[] = "-console";
	char* argv[2];

	argv[0] = exeName;
	argv[1] = consArg;
	Service::Get()->Start(2, argv);
}

bool TestMqttOwfs::CoverageConfig()
{
    mqttOwfs.SetConfigfile("./test/data/MqttOwfs1.conf");
	thread integrationTest1(ThreadConf, &mqttOwfs);
	integrationTest1.detach();
	waitMsg(5, 500);
	m_Messages.clear();
	Plateforms::delay(200);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs2.conf");
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	Plateforms::delay(200);
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	waitMsg(5, 500);
	m_Messages.clear();
	Plateforms::delay(200);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs3.conf");
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	Plateforms::delay(200);
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	waitMsg(5, 500);
	m_Messages.clear();
	Plateforms::delay(200);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs4.conf");
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	Plateforms::delay(200);
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	waitMsg(5, 500);
	m_Messages.clear();
	Plateforms::delay(200);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs5.conf");
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	Plateforms::delay(200);
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	waitMsg(5, 500);
	m_Messages.clear();
	Plateforms::delay(200);

    mqttOwfs.SetConfigfile("./test/data/MqttOwfs6.conf");
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	Plateforms::delay(200);
   	mqttClient.Publish("owfs/command", "RELOAD_CONFIG");
	waitMsg(5, 500);
	m_Messages.clear();
	Plateforms::delay(200);

	Service::Get()->ChangeStatus(Service::StatusKind::STOP);
	Plateforms::delay(100);

    return true;
}
