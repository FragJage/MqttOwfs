#ifndef MQTTOWFS_H
#define MQTTOWFS_H

#include <mutex>
#include <map>
#include "MqttDaemon.h"
#include "owDevice.h"
#include "owfscpp.h"

class MqttOwfs : public MqttDaemon
{
    public:
        MqttOwfs();
        ~MqttOwfs();

		int DaemonLoop(int argc, char* argv[]);
		void IncomingMessage(const std::string& topic, const std::string& message);

    private:
		void DaemonConfigure(SimpleIni& iniFile);
		void DevicesConfigure(SimpleIni& iniFile);

		void MessageForService(const std::string& msg);
		void MessageForDevice(const std::string& device, const std::string& msg);

		void Refresh();
		bool RefreshValue(owDevice& device);
		void RefreshValues(bool forceRefresh);
		void RefreshDevices(bool forceRefresh);

		bool OwDeviceExist(const std::string& device);
		void OwDeviceAdd(const std::string& displayName, const std::string& configName, int round);
		void OwDeviceAdd(const std::string& displayName);
		std::string OwGetValue(const std::string& configName, int round, bool uncachedRead);
		std::string OwIsPresent(const std::string& configName, bool uncachedRead);

		std::map<std::string, owDevice> m_OwDevices;
		int m_RefreshDevicesInterval;
		std::mutex m_OwfsClientAccess;
		std::mutex m_RefreshAccess;
		owfscpp m_OwfsClient;
};

#endif // MQTTOWFS_H
