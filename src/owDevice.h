#ifndef OWDEVICE_H
#define OWDEVICE_H

#include<string>

class owDevice
{
    public:
        owDevice();
        owDevice(const std::string& displayName, const std::string& deviceName, int round, bool uncachedRead);
        owDevice(const std::string& displayName, const std::string& deviceName, int round, bool uncachedRead, const std::string& current);
        ~owDevice();

        void swap(owDevice& other);
        owDevice(owDevice const& other);
        owDevice& operator=(owDevice const& other);
        owDevice(owDevice &&other);
        owDevice& operator=(owDevice&& other) noexcept;

        std::string GetDisplayName();
        int GetRound();
		bool GetUncachedRead();
        std::string GetValue();
        void SetValue(const std::string& current);

    private:
        std::string m_DisplayName;
        std::string m_DeviceName;
        int m_Round;
		bool m_UncachedRead;
        std::string m_Current;
};

#endif // OWDEVICE_H
