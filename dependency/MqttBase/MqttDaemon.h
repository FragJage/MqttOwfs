#ifndef MQTTDAEMON_H
#define MQTTDAEMON_H

#define LOG_ENTER LOG_DEBUG(m_Log) << "*** Enter ***"
#define LOG_EXIT_OK LOG_DEBUG(m_Log) << "*** Exit OK ***"
#define LOG_EXIT_KO LOG_DEBUG(m_Log) << "*** Exit KO ***"

#include<map>
#ifdef WIN32
#include <WinSock2.h>		// To stop windows.h including winsock.h
#endif
#include "MqttClient.h"
#include "Service/Service.h"
#include "SimpleIni/SimpleIni.h"
#include "SimpleLog/SimpleLog.h"

class MqttDaemon : public Service::IService, public MqttClient::IMqttMessage
{
    public:
		MqttDaemon(const std::string& topic, const std::string& configFileName);
        virtual ~MqttDaemon();

		virtual void DaemonConfigure(SimpleIni& iniFile) = 0;
		virtual int DaemonStart(int argc, char* argv[]) = 0;
		virtual void on_message(const std::string& topic, const std::string& message)=0;
		std::string GetMainTopic();
		int Subscribe(const std::string& topic);
		void Publish(const std::string& sensor, const std::string& value);
		int Loop(int timeout);
		std::string GetMqttErrorMessage(int errorNo);

		int ServiceStart(int argc, char* argv[]);
		void ServicePause(bool bPause);
		void ServiceStop();
		void SetConfigfile(const std::string& configFile);

	protected:
		SimpleLog* m_Log;
		bool m_bServicePause;
		bool m_bServiceStop;

    private:
		void SetLogLevel(const std::string& level);
		void SetLogDestination(const std::string& destination);
		void ReadParameters(int argc, char* argv[]);

		void Configure();
		void MqttConfigure(SimpleIni& iniFile);
		void LogConfigure(SimpleIni& iniFile);

		std::ofstream m_logStream;
		std::string m_logFile;
		SimpleLog m_SimpleLog;
		SimpleLog::DefaultWriter m_logWriter;
		SimpleLog::DefaultFilter m_logFilter;

		MqttClient m_MqttClient;
		std::string m_ConfigFilename;
};

#endif // MQTTDAEMON_H
