#ifndef SERVICE_H
#define SERVICE_H

#ifdef WIN32
#include <Windows.h>
#include <Winsvc.h>
#endif // WIN32
#include <cstring>
#include <string>
#include <vector>
#include <condition_variable>
#include <mutex>


class ServiceConditionVariable
{
public :
	ServiceConditionVariable() : m_Id(-1) {};
	~ServiceConditionVariable() {};
	void set_id(int id) { m_Id = id; };
	void notify_one();
	static int wait();
	static int wait_for(int timeout);
private :
	int m_Id;
	static std::condition_variable m_UniqueConditionVariable;
	static std::mutex m_UniqueMutex;
	static int m_LastId;
};

class Service
{
public:
    class Exception;
	class IService;
	enum StatusKind { START, STOP, PAUSE };

	static Service* Create(const std::string& name, const std::string& description, IService *service);
	static Service* Get();
	static void Destroy();
	static const int STATUS_CHANGED;
	static const int TIMEOUT;

	int Start(int argc, char* argv[]);
	int Wait(std::vector<std::reference_wrapper<ServiceConditionVariable>> cvs);
	int WaitFor(std::vector<std::reference_wrapper<ServiceConditionVariable>> cvs, int timeout);
	StatusKind GetStatus();
	void ChangeStatus(StatusKind status);

private:
    Service(const std::string& name, const std::string& description, IService *service);
    virtual ~Service();
	void SetIds(std::vector<std::reference_wrapper<ServiceConditionVariable>> cvs);

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
        std::string m_daemonName;
        std::string m_pidFile;
	#endif // WIN32

	char*				m_pName;
	char*				m_pDescription;
	IService*           m_iService;
	static Service*     m_pInstance;
	StatusKind					m_Status;
	std::mutex					m_StatusAccess;
	ServiceConditionVariable	m_StatusChanged;
};

class Service::IService
{
	public:
		virtual int ServiceLoop(int argc, char* argv[]) = 0;
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

