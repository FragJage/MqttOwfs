#include "owDevice.h"

using namespace std;

owDevice::owDevice() : m_DisplayName(), m_DeviceName(), m_Round(-1), m_Current()
{
}

owDevice::owDevice(const string& displayName, const string& deviceName, int round) : m_DisplayName(displayName), m_DeviceName(deviceName), m_Round(round), m_Current()
{
}

owDevice::owDevice(const string& displayName, const string& deviceName, int round, const string& current) : m_DisplayName(displayName), m_DeviceName(deviceName), m_Round(round), m_Current(current)
{
}

owDevice::~owDevice()
{
}

void owDevice::swap(owDevice& other)
{
    std::swap(m_DisplayName, other.m_DisplayName);
    std::swap(m_DeviceName, other.m_DeviceName);
    m_Round = other.m_Round;
    std::swap(m_Current, other.m_Current);
}

owDevice::owDevice(owDevice const& other):
    m_DisplayName{other.m_DisplayName},
    m_DeviceName{other.m_DeviceName},
    m_Round{other.m_Round},
    m_Current{other.m_Current}
{

}

owDevice& owDevice::operator=(owDevice const& other)
{
    owDevice{other}.swap(*this);
    return *this;
}

owDevice::owDevice(owDevice &&other)
{
    m_DisplayName = move(other.m_DisplayName);
    m_DeviceName = move(other.m_DeviceName);
    m_Round = other.m_Round;
    m_Current = move(other.m_Current);
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

int owDevice::GetRound()
{
    return m_Round;
}

string owDevice::GetValue()
{
    return m_Current;
}

void owDevice::SetValue(const string& current)
{
    m_Current = current;
}
