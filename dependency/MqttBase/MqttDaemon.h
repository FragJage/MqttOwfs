#ifndef MQTTDAEMON_H
#define MQTTDAEMON_H

#define LOG_ENTER LOG_DEBUG(m_Log) << "*** Enter ***"
#define LOG_EXIT_OK LOG_DEBUG(m_Log) << "*** Exit OK ***"
#define LOG_EXIT_KO LOG_DEBUG(m_Log) << "*** Exit KO ***"

#include<map>
#ifdef WIN32
#include <WinSock2.h>		// To stop windows.h including winsock.h
#endif
#include "MqttBase.h"
#include "Service/Service.h"
#include "SimpleIni/SimpleIni.h"
#include "SimpleLog/SimpleLog.h"

class MqttDaemon : public Service::IService, public MqttBase
{
    public:
		MqttDaemon(const std::string& topic, const std::string& configFileName);
        virtual ~MqttDaemon();

		virtual void DaemonConfigure(SimpleIni& iniFile) = 0;
		virtual int DaemonLoop(int argc, char* argv[]) = 0;
		virtual void on_message(const std::string& topic, const std::string& message)=0;

		int ServiceLoop(int argc, char* argv[]);
		void SetConfigfile(const std::string& configFile);

	protected:
		SimpleLog* m_Log;

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

		std::string m_ConfigFilename;
};

#endif // MQTTDAEMON_H
