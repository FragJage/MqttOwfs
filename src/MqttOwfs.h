#ifndef MQTTOWFS_H
#define MQTTOWFS_H

#define LOG_ENTER LOG_DEBUG(m_Log) << "*** Enter ***"
#define LOG_EXIT_OK LOG_DEBUG(m_Log) << "*** Exit OK ***"
#define LOG_EXIT_KO LOG_DEBUG(m_Log) << "*** Exit KO ***"

#include<map>
#ifdef WIN32
#include <WinSock2.h>		// To stop windows.h including winsock.h
#endif
#include "MqttClient.h"
#include "owDevice.h"
#include "Service/Service.h"
#include "SimpleIni/SimpleIni.h"
#include "SimpleLog/SimpleLog.h"
#include "owfscpp/owfscpp.h"

class MqttOwfs : public Service::IService, public MqttClient::IMqttMessage
{
    public:
        MqttOwfs();
        ~MqttOwfs();

	int ServiceStart(int argc, char* argv[]);
	void ServicePause(bool bPause);
	void ServiceStop();
	void on_message(const std::string& topic, const std::string& message);

    private:
	void SetLogLevel(const std::string& level);
	void SetLogDestination(const std::string& destination);
	void SetConfigfile(const std::string& configFile);
	void ReadParameters(int argc, char* argv[]);
		
	void Configure();
	void OwfsConfigure(SimpleIni& iniFile);
	void MqttConfigure(SimpleIni& iniFile);
	void LogConfigure(SimpleIni& iniFile);
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

	std::ofstream m_logStream;
	std::string m_logFile;
	SimpleLog* m_Log;
	SimpleLog m_SimpleLog;
	SimpleLog::DefaultWriter m_logWriter;
	SimpleLog::DefaultFilter m_logFilter;
		
	std::map<std::string, owDevice> m_OwDevices;
        int m_RefreshDevicesInterval;
        int m_RefreshValuesInterval;
        bool m_bServicePause;
        bool m_bServiceStop;
        owfscpp m_OwfsClient;
        MqttClient m_MqttClient;
	std::string m_ConfigFilename;
};

#endif // MQTTOWFS_H
