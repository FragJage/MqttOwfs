#ifndef SERVICE_H
#define SERVICE_H

#ifdef WIN32
#include <Windows.h>
#include <Winsvc.h>
#else
#include <vector>
#endif // WIN32
#include <string>

using namespace std;

class Service
{
public:
    class Exception;
	class IService;

	static Service* Create(const string& name, const string& description, IService *service);
	static Service* Get();
	static void Destroy();

	int Start(int argc, char* argv[]);

private:
    Service(const string& name, const string& description, IService *service);
    virtual ~Service();
    #ifdef WIN32
        typedef BOOL(WINAPI *ChangeServiceConfigType)(SC_HANDLE, DWORD, LPCVOID);

        int CmdInstall();
        int CmdRemove();

        void ServiceCtrlHandler(unsigned long controlCode);
        void ServiceMain(int argc, char* argv[]);

        friend void WINAPI ServiceMain(unsigned long argc, char* argv[]);
        friend void WINAPI ServiceCtrlHandler(unsigned long controlCode);

        SERVICE_STATUS			m_serviceStatus;
        SERVICE_STATUS_HANDLE	m_serviceStatusHandle;
	#else
        int CmdStart(int argc, char* argv[]);
        int CmdStop();
        int CmdRestart(int argc, char* argv[]);

        int FindPid();
        void WritePid(pid_t pid);
        void SetPidFile();


        static std::vector<std::string> m_runDirs;
        string m_daemonName;
        string m_pidFile;
	#endif // WIN32

	char*				m_pName;
	char*				m_pDescription;
	IService*           m_iService;
	static Service*     m_pInstance;
};

class Service::IService
{
	public:
		virtual int ServiceStart(int argc, char* argv[]) = 0;
		virtual void ServicePause(bool bPause) = 0;
		virtual void ServiceStop() = 0;
};

class Service::Exception: public std::exception
{
    public:
        Exception(int number, std::string const& message) throw();
        ~Exception() throw();
        const char* what() const throw();
        int GetNumber() const throw();

    private:
        int m_number;
        std::string m_message;
};

#endif // SERVICE_H

