#ifndef MQTTOWFS_H
#define MQTTOWFS_H

#include<map>
#include "MqttBase/MqttDaemon.h"
#include "owDevice.h"
#include "owfscpp/owfscpp.h"

class MqttOwfs : public MqttDaemon
{
    public:
        MqttOwfs();
        ~MqttOwfs();

		int DaemonStart(int argc, char* argv[]);
		void on_message(const std::string& topic, const std::string& message);

    private:
		void DaemonConfigure(SimpleIni& iniFile);
		void DevicesConfigure(SimpleIni& iniFile);

		void MessageForService(const std::string& msg);
		void MessageForDevice(const std::string& device, const std::string& msg);

		void Refresh();
		void RefreshValue(const std::string& name, owDevice& device);
		void RefreshValues();
		void RefreshDevices();

		bool OwDeviceExist(const std::string& device);
		void OwDeviceAdd(const std::string& displayName, const std::string& configName, int round);
		void OwDeviceAdd(const std::string& displayName);
		std::string OwGetValue(const std::string& configName, int round);

		std::map<std::string, owDevice> m_OwDevices;
		int m_RefreshDevicesInterval;
		int m_RefreshValuesInterval;
		owfscpp m_OwfsClient;
};

#endif // MQTTOWFS_H
