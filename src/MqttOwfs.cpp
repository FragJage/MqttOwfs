#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "MqttOwfs.h"
#include "Plateforms/Plateforms.h"
#include "SimpleFolders/SimpleFolders.h"


using namespace std;


MqttOwfs::MqttOwfs() : MqttDaemon("owfs", ".MqttOwfs"), m_RefreshDevicesInterval(90), m_RefreshValuesInterval(10), m_OwfsClient()
{
	m_OwfsClient.Initialisation("127.0.0.1", 4304);
}

MqttOwfs::~MqttOwfs()
{
    m_OwDevices.clear();
}

void MqttOwfs::DaemonConfigure(SimpleIni& iniFile)
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

	m_OwDevices.clear();
	DevicesConfigure(iniFile);
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
    Publish(displayName, value);
}

void MqttOwfs::OwDeviceAdd(const string& name)
{
	string svalue;
	int family;


	try
	{
		svalue = m_OwfsClient.Get(name + "/family");
	}
	catch (const exception& e)
	{
		LOG_WARNING(m_Log) << "Unable to read family of device " << name << " : " << e.what();
		return;
	}

	istringstream iss(svalue);
	iss >> hex >> family;

    switch(family)
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

void MqttOwfs::RefreshValue(const string& name, owDevice& device)
{
    string value;

        value = OwGetValue(name, device.GetRound());
        if(value==device.GetValue()) return;
        device.SetValue(value);
        Publish(device.GetDisplayName(), value);
}

void MqttOwfs::RefreshValues()
{
    std::map<std::string, owDevice>::iterator it;


    for(it=m_OwDevices.begin(); it!=m_OwDevices.end(); ++it)
	RefreshValue(it->first, it->second);
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
			Publish(it->second.GetDisplayName(), it->second.GetValue());
	}
	else if (msg == "REFRESH_DEVICES")
	{
		RefreshDevices();
	}
	else if (msg == "REFRESH_VALUES")
	{
		RefreshValues();
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
		Publish(it->second.GetDisplayName(), it->second.GetValue());
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

	RefreshValue(it->first, it->second);

	return;
}

void MqttOwfs::on_message(const string& topic, const string& message)
{
	LOG_VERBOSE(m_Log) << "Mqtt receive " << topic << " : " << message;

	string mainTopic = GetMainTopic();
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

	string device = topic.substr(mainTopic.length() + 8);
	return MessageForDevice(device, message);
}

int MqttOwfs::DaemonStart(int argc, char* argv[])
{
	int ret;

	LOG_ENTER;
	RefreshDevices();

	ret = Subscribe(GetMainTopic() + "command/#");
	if (ret != 0)
		LOG_ERROR(m_Log) << "Unable to subscript to mqtt server : " << GetMqttErrorMessage(ret);
	else
		LOG_VERBOSE(m_Log) << "Subscript to : " << GetMainTopic() + "command/#";

	for (map<string, owDevice>::iterator it = m_OwDevices.begin(); it != m_OwDevices.end(); ++it)
		Publish(it->second.GetDisplayName(), it->second.GetValue());

    while(!m_bServiceStop)
    {
        if(m_bServicePause)
        {
            Plateforms::delay(500);
        }
        else
        {
			Refresh();
			Loop(500);
        }
    }

	LOG_EXIT_OK;
    return 0;
}