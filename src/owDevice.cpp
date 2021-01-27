#include "owDevice.h"
#include "StringTools.h"

using namespace std;

bool owDevice::m_DefaultUncachedRead = false;
int owDevice::m_DefaultRefreshInterval = 10;

owDevice::owDevice() : owDevice("", "", -1, "")
{
}

owDevice::owDevice(const string& displayName, const string& deviceName, int round) : owDevice(displayName, deviceName, round, "")
{
}

owDevice::owDevice(const string& displayName, const string& deviceName, int round, const string& current) :
    m_DisplayName(displayName),
    m_DeviceName(deviceName),
    m_Round(round),
    m_Current(current),
    m_OnlyPresence(false),
    m_UncachedRead(owDevice::m_DefaultUncachedRead),
    m_RefreshInterval(owDevice::m_DefaultRefreshInterval),
    m_LastRefresh(time((time_t*)0))
{
    int pos = m_DeviceName.length()-10;
    if((pos>0) && (StringTools::IsEqualCaseInsensitive(m_DeviceName.substr(pos), "/IsPresent")))
    {
        m_DeviceName = m_DeviceName.substr(0, pos);
        m_OnlyPresence = true;
    }
}

owDevice::~owDevice()
{
}

void owDevice::swap(owDevice& other)
{
    std::swap(m_DisplayName, other.m_DisplayName);
    std::swap(m_DeviceName, other.m_DeviceName);
    std::swap(m_Round, other.m_Round);
    std::swap(m_Current, other.m_Current);
	std::swap(m_UncachedRead, other.m_UncachedRead);
	std::swap(m_RefreshInterval, other.m_RefreshInterval);
	std::swap(m_LastRefresh, other.m_LastRefresh);
}

owDevice::owDevice(owDevice const& other):
    m_DisplayName{other.m_DisplayName},
    m_DeviceName{other.m_DeviceName},
    m_Round{other.m_Round},
    m_Current{other.m_Current},
	m_UncachedRead{other.m_UncachedRead},
	m_RefreshInterval{other.m_RefreshInterval},
	m_LastRefresh{other.m_LastRefresh}
{
}

owDevice& owDevice::operator=(owDevice const& other)
{
    owDevice{other}.swap(*this);
    return *this;
}

owDevice::owDevice(owDevice &&other) :
    m_DisplayName(move(other.m_DisplayName)),
    m_DeviceName(move(other.m_DeviceName)),
    m_Round(other.m_Round),
    m_Current(move(other.m_Current)),
	m_UncachedRead(other.m_UncachedRead),
	m_RefreshInterval(other.m_RefreshInterval),
	m_LastRefresh(other.m_LastRefresh)
{
}

owDevice& owDevice::operator=(owDevice&& other) noexcept
{
   owDevice(move(other)).swap(*this);
   return *this;
}

string owDevice::GetDisplayName()
{
    return m_DisplayName;
}

string owDevice::GetDeviceName()
{
    return m_DeviceName;
}

int owDevice::GetRound()
{
    return m_Round;
}

bool owDevice::OnlyPresence()
{
    return m_OnlyPresence;
}

bool owDevice::GetUncachedRead()
{
	return m_UncachedRead;
}

string owDevice::GetValue()
{
    return m_Current;
}

void owDevice::SetValue(const string& current)
{
    m_Current = current;
}

void owDevice::SetUncachedRead(bool uncachedRead)
{
    m_UncachedRead = uncachedRead;
}

void owDevice::SetRefreshInterval(int refreshInterval)
{
    m_RefreshInterval = refreshInterval;
}

int owDevice::GetRefreshInterval()
{
    return m_RefreshInterval;
}

void owDevice::SetDefaultUncachedRead(bool uncachedRead)
{
    owDevice::m_DefaultUncachedRead = uncachedRead;
}

void owDevice::SetDefaultRefreshInterval(int refreshInterval)
{
    owDevice::m_DefaultRefreshInterval = refreshInterval;
}

bool owDevice::RefreshNeeded()
{
	return (time((time_t*)0)-m_LastRefresh>=m_RefreshInterval);
}

void owDevice::IsRefreshed()
{
    m_LastRefresh = time((time_t*)0);
}

void owDevice::ExtendRefreshDelay(int delay)
{
    m_LastRefresh = time((time_t*)0)-m_RefreshInterval+delay;
}
