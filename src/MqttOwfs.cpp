#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "MqttOwfs.h"
#include "Plateforms/Plateforms.h"
#include "SimpleFolders/SimpleFolders.h"


using namespace std;


MqttOwfs::MqttOwfs() : m_logFile(""), m_RefreshDevicesInterval(90), m_RefreshValuesInterval(10), m_bServicePause(false), m_bServiceStop(false), m_OwfsClient(), m_MqttClient()
{
	m_Log = &m_SimpleLog;
	m_SimpleLog.SetFilter(&m_logFilter);
	m_SimpleLog.SetWriter(&m_logWriter);
	
	m_ConfigFilename = SimpleFolders::AddFile(SimpleFolders::GetFolder(SimpleFolders::FolderType::User), ".MqttOwfs", "conf");
	if (!SimpleFolders::FileExists(m_ConfigFilename)) m_ConfigFilename = "";
	
	m_OwfsClient.Initialisation("127.0.0.1", 4304);
	m_MqttClient.SetMainTopic("owfs");
}

MqttOwfs::~MqttOwfs()
{
    m_OwDevices.clear();
	if (m_logStream) m_logStream.close();
}

void MqttOwfs::SetConfigfile(const string& configFile)
{
	m_ConfigFilename = configFile;
}

void MqttOwfs::SetLogLevel(const string& slevel)
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

void MqttOwfs::SetLogDestination(const std::string& sdestination)
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

void MqttOwfs::Configure()
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
	OwfsConfigure(iniFile);
	MqttConfigure(iniFile);

	m_OwDevices.clear();
	DevicesConfigure(iniFile);
}

void MqttOwfs::OwfsConfigure(SimpleIni& iniFile)
{
	string svalue;
	int ivalue;
	char chvalue;

	svalue = iniFile.GetValue("owfs", "server", "127.0.0.1");
	ivalue = iniFile.GetValue("owfs", "port", 4304);
	m_OwfsClient.Initialisation(svalue, ivalue);
	LOG_VERBOSE(m_Log) << "Connect to owserver " << svalue << ":" << ivalue;

	ivalue = iniFile.GetValue("owfs", "devicesinterval", 90);
	m_RefreshDevicesInterval = ivalue;
	LOG_VERBOSE(m_Log) << "Set RefreshDevicesInterval to " << m_RefreshDevicesInterval;

	ivalue = iniFile.GetValue("owfs", "valuesinterval", 10);
	m_RefreshValuesInterval = ivalue;
	LOG_VERBOSE(m_Log) << "Set RefreshValuesInterval to " << m_RefreshValuesInterval;

	svalue = iniFile.GetValue("owfs", "temperaturescale", "C");
	chvalue = toupper(svalue.front());
	switch (chvalue)
	{
	case 'C':
		m_OwfsClient.SetTemperatureScale(owfscpp::Centigrade);
		LOG_VERBOSE(m_Log) << "Set TemperatureScale to Centigrade";
		break;
	case 'F':
		m_OwfsClient.SetTemperatureScale(owfscpp::Fahrenheit);
		LOG_VERBOSE(m_Log) << "Set TemperatureScale to Fahrenheit";
		break;
	case 'K':
		m_OwfsClient.SetTemperatureScale(owfscpp::Kelvin);
		LOG_VERBOSE(m_Log) << "Set TemperatureScale to Kelvin";
		break;
	case 'R':
		m_OwfsClient.SetTemperatureScale(owfscpp::Rankine);
		LOG_VERBOSE(m_Log) << "Set TemperatureScale to Rankine";
		break;
	default:
		LOG_WARNING(m_Log) << "Unknown TemperatureScale " << chvalue << " (Possible values C,F,K,R)";
	}

	svalue = iniFile.GetValue("owfs", "pressurescale", "Mbar");
	std::transform(svalue.begin(), svalue.end(), svalue.begin(), ::toupper);
	if (svalue == "MBAR")
	{
		m_OwfsClient.SetPressureScale(owfscpp::Mbar);
		LOG_VERBOSE(m_Log) << "Set PressureScale to Mbar";
	}
	else if (svalue == "ATM")
	{
		m_OwfsClient.SetPressureScale(owfscpp::Atm);
		LOG_VERBOSE(m_Log) << "Set PressureScale to Atm";
	}
	else if (svalue == "MMHG")
	{
		m_OwfsClient.SetPressureScale(owfscpp::MmHg);
		LOG_VERBOSE(m_Log) << "Set PressureScale to MmHg";
	}
	else if (svalue == "INHG")
	{
		m_OwfsClient.SetPressureScale(owfscpp::InHg);
		LOG_VERBOSE(m_Log) << "Set PressureScale to InHg";
	}
	else if (svalue == "PSI")
	{
		m_OwfsClient.SetPressureScale(owfscpp::Psi);
		LOG_VERBOSE(m_Log) << "Set PressureScale to Psi";
	}
	else if (svalue == "PA")
	{
		m_OwfsClient.SetPressureScale(owfscpp::Pa);
		LOG_VERBOSE(m_Log) << "Set PressureScale to Pa";
	}
	else
	{
		LOG_WARNING(m_Log) << "Unknown PressureScale " << svalue << " (Possible values Mbar,Atm,MmHg,InHg,Psi,Pa)";
	}

	svalue = iniFile.GetValue("owfs", "uncachedread", "FALSE");
	std::transform(svalue.begin(), svalue.end(), svalue.begin(), ::toupper);
	if ((svalue == "1") || (svalue == "TRUE") || (svalue == "YES"))
	{
		m_OwfsClient.SetOwserverFlag(owfscpp::Uncached, true);
		LOG_VERBOSE(m_Log) << "Set Uncached read";
	}
}

void MqttOwfs::MqttConfigure(SimpleIni& iniFile)
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

void MqttOwfs::LogConfigure(SimpleIni& iniFile)
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

void MqttOwfs::DevicesConfigure(SimpleIni& iniFile)
{
	string configName;
	string displayName;
	size_t pos;
	int round;

	for (SimpleIni::SectionIterator itSection = iniFile.beginSection(); itSection != iniFile.endSection(); ++itSection)
	{
		if ((*itSection) == "owfs") continue;
		if ((*itSection) == "mqtt") continue;
		if ((*itSection) == "log") continue;

		configName = (*itSection);
		pos = configName.find("/");
		if (pos == string::npos)
		{
			LOG_ERROR(m_Log) << "Device " << configName << " is improperly formatted, it must contain a '/'.";
			continue;
		}

		displayName = iniFile.GetValue(configName, "displayname", "");
		if (displayName == "")
		{
			LOG_ERROR(m_Log) << "Device " << configName << " have not display name";
			continue;
		}

		round = iniFile.GetValue(configName, "round", -1);

		OwDeviceAdd(displayName, configName, round);
	}
}

void MqttOwfs::RefreshDevices()
{
    list<string> lstDir;
    list<string>::iterator iDir;
    string device;


    try
    {
        lstDir = m_OwfsClient.DirAll("/");
    }
    catch (const exception& e)
    {
        LOG_ERROR(m_Log) << "Unable to find owdevices : " << e.what();
    }

    for(iDir = lstDir.begin(); iDir != lstDir.end(); ++iDir)
    {
        if(iDir->substr(0,1)=="/")
            device = iDir->substr(1);
        else
            device = iDir->substr(0);

        if(!OwDeviceExist(device))
            OwDeviceAdd(device);
    }
}


bool MqttOwfs::OwDeviceExist(const string& device)
{
    std::map<std::string, owDevice>::iterator it;
    size_t length;


    length = device.length();
    for(it=m_OwDevices.begin(); it!=m_OwDevices.end(); ++it)
    {
        if(it->first.compare(0, length, device)==0)
            return true;
    }

    return false;
}

string MqttOwfs::OwGetValue(const string& configName, int round)
{
    string svalue;
    double dvalue;
    ostringstream s;


    try
    {
        svalue = m_OwfsClient.Get(configName);
    }
    catch (const exception& e)
    {
        LOG_WARNING(m_Log) << "Unable to read " << configName << " : " << e.what();
        return "";
    }

    if(round<0) return svalue;
    dvalue = atof(svalue.c_str());
    s << fixed << setprecision(round) << dvalue;

    return s.str();
}

void MqttOwfs::OwDeviceAdd(const string& displayName, const string& configName, int round)
{
    string value;


    value = OwGetValue(configName, round);

    LOG_VERBOSE(m_Log) << "Device created " << displayName <<" : "<< configName <<" = "<< value;
    m_OwDevices[configName] = owDevice(displayName, configName, round, value);
    m_MqttClient.Publish(displayName, value);
}

void MqttOwfs::OwDeviceAdd(const string& name)
{
    switch(strtol(name.substr(0,2).c_str(), nullptr, 16))
    {
		case 0x05 : 	//DS2405
		    OwDeviceAdd(name, name+"/PIO", -1);
			break;
		case 0x10 :		//DS18S20, DS1920
		    OwDeviceAdd(name, name+"/temperature9", 1);
			break;
		case 0x12 :		//DS2406/07
		    OwDeviceAdd(name, name+"/PIO.A", -1);
			break;
		case 0x1D :		//DS2423
		    OwDeviceAdd(name, name+"/counters.A", -1);
			break;
		case 0x20 : 	//DS2450
		    OwDeviceAdd(name, name+"/PIO.A", -1);
			break;
		case 0x21 :		//DS1921
		    OwDeviceAdd(name, name+"/temperature9", 1);
			break;
		case 0x22 :		//DS1822
		    OwDeviceAdd(name, name+"/temperature9", 1);
			break;
		case 0x26 :		//DS2438
		    OwDeviceAdd(name, name+"/VDD", 1);
			break;
		case 0x28 : 	//DS18B20
		    OwDeviceAdd(name, name+"/temperature9", 1);
			break;
		case 0x29 : 	//DS2408
		    OwDeviceAdd(name, name+"/PIO.BYTE", -1);
			break;
		case 0x3A : 	//DS2413
		    OwDeviceAdd(name, name+"/PIO.A", -1);
			break;
    }

    LOG_VERBOSE(m_Log) << "Device found " << name;
}

void MqttOwfs::RefreshValues()
{
    std::map<std::string, owDevice>::iterator it;
    string value;


    for(it=m_OwDevices.begin(); it!=m_OwDevices.end(); ++it)
    {
        value = OwGetValue(it->first, it->second.GetRound());
        if(value==it->second.GetValue()) continue;
        it->second.SetValue(value);
        m_MqttClient.Publish(it->second.GetDisplayName(), value);
    }
}

void MqttOwfs::Refresh()
{
	 time_t timeNow = time((time_t*)0);
	 static time_t lastRefreshDevices = timeNow;
	 static time_t lastRefreshValues = timeNow;


	if(timeNow-lastRefreshDevices>=m_RefreshDevicesInterval)
    {
        lastRefreshDevices=timeNow;
        LOG_VERBOSE(m_Log) << "Refresh devices";
		RefreshDevices();
    }

	if(timeNow-lastRefreshValues>=m_RefreshValuesInterval)
    {
        lastRefreshValues=timeNow;
        LOG_VERBOSE(m_Log) << "Refresh values";
        RefreshValues();
    }
}

void MqttOwfs::MessageForService(const string& msg)
{
	if (msg == "REQUEST")
	{
		for (map<string, owDevice>::iterator it = m_OwDevices.begin(); it != m_OwDevices.end(); ++it)
			m_MqttClient.Publish(it->second.GetDisplayName(), it->second.GetValue());
	}
	else if (msg == "REFRESH_DEVICES")
	{
		RefreshDevices();
	}
	else if (msg == "REFRESH_VALUES")
	{
		RefreshValues();
	}
	else if (msg == "RELOAD_CONFIG")
	{
		LOG_WARNING(m_Log) << "RELOAD_CONFIG is an experimental command.";
		m_bServicePause = true;
		Plateforms::delay(500);
		m_MqttClient.Close();
		Configure();
		m_MqttClient.Open();
		m_bServicePause = false;
	}
	else
	{
		LOG_WARNING(m_Log) << "Unknown command for service " << msg;
	}
	return;
}

void MqttOwfs::MessageForDevice(const string& device, const string& msg)
{
	map<string, owDevice>::iterator it;

	for (it = m_OwDevices.begin(); it != m_OwDevices.end(); ++it)
	{
		if (it->second.GetDisplayName() == device) break;
	}
	
	if (it == m_OwDevices.end())
	{
		LOG_WARNING(m_Log) << "Unknown device " << device;
		return;
	}

	if (msg == "REQUEST")
	{
		m_MqttClient.Publish(it->second.GetDisplayName(), it->second.GetValue());
		return;
	}

	if (msg.length() > 20)
	{
		LOG_WARNING(m_Log) << "Message seem to be very long, truncate to 20 characters.";
		m_OwfsClient.Write(it->first, msg.substr(0, 20));
	}
	else
	{
		m_OwfsClient.Write(it->first, msg);
	}

	return;
}

void MqttOwfs::on_message(const string& topic, const string& message)
{
	LOG_VERBOSE(m_Log) << "Mqtt receive " << topic << " : " << message;
	
	string mainTopic = m_MqttClient.GetMainTopic();
	if (topic.substr(0, mainTopic.length()) != mainTopic)
	{
		LOG_WARNING(m_Log) << "Not for me (" << mainTopic << ")";
		return;
	}

	if (topic.substr(mainTopic.length(), 7) != "command")
	{
		LOG_WARNING(m_Log) << "Not a command (waiting " << mainTopic+"command" << ")";
		return;
	}

	if (topic.length() == mainTopic.length() + 7) return MessageForService(message);

	string device = topic.substr(mainTopic.length() + 7);
	return MessageForDevice(device, message);
}

void MqttOwfs::ReadParameters(int argc, char* argv[])
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

int MqttOwfs::ServiceStart(int argc, char* argv[])
{
	int ret;

	LOG_ENTER;
    m_bServiceStop = false;

	ReadParameters(argc, argv);
	Configure();
	RefreshDevices();

	ret = m_MqttClient.Open();
	if (ret != 0)
	{
		LOG_FATAL(m_Log) << "Unable to connect to mqtt server : " << m_MqttClient.GetErrorMessage(ret);
		LOG_EXIT_KO;
		return ret;
	}

	m_MqttClient.SetMessageCallback(this);
	ret = m_MqttClient.Subscribe(m_MqttClient.GetMainTopic() + "command/#");
	if (ret != 0)
		LOG_ERROR(m_Log) << "Unable to subscript to mqtt server : " << m_MqttClient.GetErrorMessage(ret);
	else
		LOG_VERBOSE(m_Log) << "Subscript to : " << m_MqttClient.GetMainTopic() + "command/#";

	for (map<string, owDevice>::iterator it = m_OwDevices.begin(); it != m_OwDevices.end(); ++it)
		m_MqttClient.Publish(it->second.GetDisplayName(), it->second.GetValue());

    while(!m_bServiceStop)
    {
        if(m_bServicePause)
        {
            Plateforms::delay(500);
        }
        else
        {
			Refresh();
			m_MqttClient.Loop(500);
        }
    }

    m_MqttClient.Close();
	LOG_EXIT_OK;
    return 0;
}

void MqttOwfs::ServicePause(bool bPause)
{
    m_bServicePause = bPause;
}

void MqttOwfs::ServiceStop()
{
    m_bServiceStop = true;
}
