#ifndef OWDEVICE_H
#define OWDEVICE_H

#include <string>
#include <time.h>

class owDevice
{
    public:
        owDevice();
        owDevice(const std::string& displayName, const std::string& deviceName, int round);
        owDevice(const std::string& displayName, const std::string& deviceName, int round, const std::string& current);
        ~owDevice();

        void swap(owDevice& other);
        owDevice(owDevice const& other);
        owDevice& operator=(owDevice const& other);
        owDevice(owDevice &&other);
        owDevice& operator=(owDevice&& other) noexcept;

        std::string GetDisplayName();
        std::string GetDeviceName();
        int GetRound();
		bool GetUncachedRead();
        std::string GetValue();
        bool OnlyPresence();

        void SetValue(const std::string& current);
        void SetUncachedRead(bool uncachedRead);
        void SetRefreshInterval(int refreshInterval);
        bool RefreshNeeded();
        void IsRefreshed();

        static void SetDefaultUncachedRead(bool uncachedRead);
        static void SetDefaultRefreshInterval(int refreshInterval);

    private:
        std::string m_DisplayName;
        std::string m_DeviceName;
        int m_Round;
        std::string m_Current;
        bool m_OnlyPresence;
		bool m_UncachedRead;
        int m_RefreshInterval;
        time_t m_LastRefresh;

        static bool m_DefaultUncachedRead;
        static int m_DefaultRefreshInterval;
};

#endif // OWDEVICE_H
