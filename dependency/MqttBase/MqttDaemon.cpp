#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "MqttDaemon.h"
#include "Plateforms/Plateforms.h"
#include "SimpleFolders/SimpleFolders.h"


using namespace std;


MqttDaemon::MqttDaemon(const string& topic, const string& configFileName) : m_bServicePause(false), m_bServiceStop(false), m_logFile(""), m_MqttClient()
{
	m_Log = &m_SimpleLog;
	m_SimpleLog.SetFilter(&m_logFilter);
	m_SimpleLog.SetWriter(&m_logWriter);

	m_ConfigFilename = SimpleFolders::AddFile(SimpleFolders::GetFolder(SimpleFolders::FolderType::User), configFileName, "conf");
	if (!SimpleFolders::FileExists(m_ConfigFilename)) m_ConfigFilename = "";

	m_MqttClient.SetMainTopic(topic);
}

MqttDaemon::~MqttDaemon()
{
	if (m_logStream) m_logStream.close();
}

void MqttDaemon::SetConfigfile(const string& configFile)
{
	m_ConfigFilename = configFile;
}

void MqttDaemon::SetLogLevel(const string& slevel)
{
	string level = slevel;

	std::transform(level.begin(), level.end(), level.begin(), ::toupper);

	if ((level == "1") || (level == "FATAL"))
	{
		m_logFilter.SetLevel(SimpleLog::LVL_FATAL);
		LOG_VERBOSE(m_Log) << "Set log level to Fatal";
	}
	else if ((level == "2") || (level == "ERROR"))
	{
		m_logFilter.SetLevel(SimpleLog::LVL_ERROR);
		LOG_VERBOSE(m_Log) << "Set log level to Error";
	}
	else if ((level == "3") || (level == "WARNING"))
	{
		m_logFilter.SetLevel(SimpleLog::LVL_WARNING);
		LOG_VERBOSE(m_Log) << "Set log level to Warning";
	}
	else if ((level == "4") || (level == "INFO"))
	{
		m_logFilter.SetLevel(SimpleLog::LVL_INFO);
		LOG_VERBOSE(m_Log) << "Set log level to Info";
	}
	else if ((level == "5") || (level == "DEBUG"))
	{
		m_logFilter.SetLevel(SimpleLog::LVL_DEBUG);
		LOG_VERBOSE(m_Log) << "Set log level to Debug";
	}
	else if ((level == "6") || (level == "VERBOSE"))
	{
		m_logFilter.SetLevel(SimpleLog::LVL_VERBOSE);
		LOG_VERBOSE(m_Log) << "Set log level to Verbose";
	}
	else if ((level == "7") || (level == "TRACE"))
	{
		m_logFilter.SetLevel(SimpleLog::LVL_TRACE);
		LOG_VERBOSE(m_Log) << "Set log level to Trace";
	}
	else
	{
		m_logFilter.SetLevel(SimpleLog::LVL_WARNING);
		LOG_WARNING(m_Log) << "Unknown debug level " << level << " (Possible values Fatal,Error,Warning,Info,Debug,Verbose,Trace)";
	}
}

void MqttDaemon::SetLogDestination(const std::string& sdestination)
{
	string destination = sdestination;

	std::transform(destination.begin(), destination.end(), destination.begin(), ::toupper);
	if (destination == "COUT")
	{
		m_logWriter.SetStream(cout);
		LOG_VERBOSE(m_Log) << "Set log destination to std::cout";
	}
	else if (destination == "CERR")
	{
		m_logWriter.SetStream(cerr);
		LOG_VERBOSE(m_Log) << "Set log destination to std::cerr";
	}
	else if (destination == "CLOG")
	{
		m_logWriter.SetStream(clog);
		LOG_VERBOSE(m_Log) << "Set log destination to std::clog";
	}
	else
	{
		if (m_logStream) m_logStream.close();
		m_logStream.open(sdestination, ios_base::app);
		m_logWriter.SetStream(m_logStream);
		LOG_VERBOSE(m_Log) << "Set log destination to file " << sdestination;
	}
}

void MqttDaemon::Configure()
{
	SimpleIni iniFile;

	if (m_ConfigFilename == "")
	{
		LOG_INFO(m_Log) << "No config file";
		return;
	}

	LOG_INFO(m_Log) << "Load config (file " << m_ConfigFilename << ")";

	iniFile.SetOptions(iniFile.Comment, "#");
	if (!iniFile.Load(m_ConfigFilename))
	{
		LOG_INFO(m_Log) << "Unable to open config file.";
		return;
	}

	LogConfigure(iniFile);
	MqttConfigure(iniFile);
}

void MqttDaemon::MqttConfigure(SimpleIni& iniFile)
{
	string svalue;
	int ivalue;

	svalue = iniFile.GetValue("mqtt", "server", "127.0.0.1");
	ivalue = iniFile.GetValue("mqtt", "port", 1883);
	m_MqttClient.SetServer(svalue, ivalue);
	LOG_VERBOSE(m_Log) << "Connect to mqtt server " << svalue << ":" << ivalue;

	ivalue = iniFile.GetValue("mqtt", "keepalive", 300);
	m_MqttClient.SetKeepAlive(ivalue);
	LOG_VERBOSE(m_Log) << "Set mqtt keepalive to " << ivalue;

	svalue = iniFile.GetValue("mqtt", "topic", "owfs");
	m_MqttClient.SetMainTopic(svalue);
	LOG_VERBOSE(m_Log) << "Set mqtt topic to " << svalue;
}

void MqttDaemon::LogConfigure(SimpleIni& iniFile)
{
	string svalue;

	svalue = iniFile.GetValue("log", "level", "ERROR");
	SetLogLevel(svalue);

	svalue = iniFile.GetValue("log", "destination", "CLOG");
	SetLogDestination(svalue);

	svalue = iniFile.GetValue("log", "module", "");
	if (svalue != "")
	{
		m_logFilter.SetModule(svalue);
		LOG_VERBOSE(m_Log) << "Set log Module to " << svalue;
	}

	svalue = iniFile.GetValue("log", "function", "");
	if (svalue != "")
	{
		m_logFilter.SetFunction(svalue);
		LOG_VERBOSE(m_Log) << "Set log Function to " << svalue;
	}
}

void MqttDaemon::ReadParameters(int argc, char* argv[])
{
	int i;
	char** arg;

	arg = argv; arg++;
	for (i = 1; i < argc; ++i, ++arg)
	{
		if ((strcmp("-f", *arg) == 0) || (strcmp("--configfile", *arg) == 0))
		{
			SetConfigfile(*(arg + 1));
		}
		else if ((strcmp("-l", *arg) == 0) || (strcmp("--loglevel", *arg) == 0))
		{
			SetLogLevel(*(arg + 1));
		}
		else if ((strcmp("-d", *arg) == 0) || (strcmp("--logdestination", *arg) == 0))
		{
			SetLogDestination(*(arg + 1));
		}
	}
}

string MqttDaemon::GetMainTopic()
{
	return m_MqttClient.GetMainTopic();
}

int MqttDaemon::Subscribe(const string& topic)
{
	return m_MqttClient.Subscribe(topic);
}

void MqttDaemon::Publish(const string& sensor, const string& value)
{
	m_MqttClient.Publish(sensor, value);
}

int MqttDaemon::Loop(int timeout)
{
	return m_MqttClient.Loop(timeout);
}

string MqttDaemon::GetMqttErrorMessage(int errorNo)
{
	return m_MqttClient.GetErrorMessage(errorNo);
}

int MqttDaemon::ServiceStart(int argc, char* argv[])
{
	int ret;

	LOG_ENTER;
    m_bServiceStop = false;

	ReadParameters(argc, argv);
	Configure();

	ret = m_MqttClient.Open();
	if (ret != 0)
	{
		LOG_FATAL(m_Log) << "Unable to connect to mqtt server : " << m_MqttClient.GetErrorMessage(ret);
		LOG_EXIT_KO;
		return ret;
	}

	m_MqttClient.SetMessageCallback(this);
	ret = DaemonStart(argc, argv);

    m_MqttClient.Close();
	LOG_EXIT_OK;
    return 0;
}

void MqttDaemon::ServicePause(bool bPause)
{
    m_bServicePause = bPause;
}

void MqttDaemon::ServiceStop()
{
    m_bServiceStop = true;
}
