#ifndef MQTTOWFS_H
#define MQTTOWFS_H

#include <mutex>
#include <queue>
#include <map>
#include "MqttDaemon.h"
#include "owDevice.h"
#include "owfscpp.h"

struct MqttQueue
{
	MqttQueue(std::string topic, std::string msg) : Topic(topic), Message(msg) {};
	std::string Topic;
	std::string Message;
};

class MqttOwfs : public MqttDaemon
{
    public:
        MqttOwfs();
        ~MqttOwfs();

		int DaemonLoop(int argc, char* argv[]);
		void on_message(const std::string& topic, const std::string& message);

    private:
		void DaemonConfigure(SimpleIni& iniFile);
		void DevicesConfigure(SimpleIni& iniFile);

		void MessageForService(const std::string& msg);
		void MessageForDevice(const std::string& device, const std::string& msg);
		void SendMqttMessages();

		bool RefreshValue(const std::string& name, owDevice& device);
		void RefreshValues(bool forceRefresh);
		void RefreshDevices(bool forceRefresh);

		bool OwDeviceExist(const std::string& device);
		void OwDeviceAdd(const std::string& displayName, const std::string& configName, int round);
		void OwDeviceAdd(const std::string& displayName);
		std::string OwGetValue(const std::string& configName, int round, bool uncachedRead);

		std::map<std::string, owDevice> m_OwDevices;
		int m_RefreshDevicesInterval;
		owfscpp m_OwfsClient;

		std::mutex m_MqttQueueAccess;
		ServiceConditionVariable m_MqttQueueCond;
		ServiceConditionVariable m_RestartCond;
		std::queue<MqttQueue> m_MqttQueue;
};

#endif // MQTTOWFS_H
