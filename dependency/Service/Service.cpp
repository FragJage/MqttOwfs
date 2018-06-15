#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include "Service.h"

using namespace std;

condition_variable ServiceConditionVariable::m_UniqueConditionVariable;
mutex ServiceConditionVariable::m_UniqueMutex;
int ServiceConditionVariable::m_LastId = -1;

#ifdef WIN32
void WINAPI ServiceMain(unsigned long argc, char* argv[])
{
	Service* pService = Service::Get();
	if(pService==NULL) return;
	pService->ServiceMain(argc, argv);
	return;
}

void WINAPI ServiceCtrlHandler(unsigned long controlCode)
{
	Service* pService = Service::Get();
	if(pService==NULL) return;
	pService->ServiceCtrlHandler(controlCode);
	return;
}
#else
#include <signal.h>

void service_signal_handler(int signal)
{
	switch (signal)
	{
	case SIGINT:
	case SIGABRT:
	case SIGQUIT:
	case SIGKILL:
	case SIGTERM:
		Service::Get()->ChangeStatus(Service::StatusKind::STOP);
		break;
	case SIGCONT:
		Service::Get()->ChangeStatus(Service::StatusKind::START);
		break;
	case SIGTSTP:
		Service::Get()->ChangeStatus(Service::StatusKind::PAUSE);
		break;
	}
}
#endif

/**************************************************************************************************************/
/***                                                                                                        ***/
/*** Class ServiceConditionVariable                                                                         ***/
/***                                                                                                        ***/
/**************************************************************************************************************/
void ServiceConditionVariable::notify_one()
{
	lock_guard<mutex> lock(m_UniqueMutex); 
	m_LastId = m_Id; 
	m_UniqueConditionVariable.notify_one();
};

int ServiceConditionVariable::wait()
{
	unique_lock<mutex> lock(m_UniqueMutex);
	m_UniqueConditionVariable.wait(lock);
	return m_LastId;
};

int ServiceConditionVariable::wait_for(int timeout)
{
	unique_lock<mutex> lock(m_UniqueMutex);
	if (m_UniqueConditionVariable.wait_for(lock, timeout * chrono::milliseconds(1)) == cv_status::timeout) m_LastId = Service::TIMEOUT;
	return m_LastId;
};

/**************************************************************************************************************/
/***                                                                                                        ***/
/*** Class Service                                                                                          ***/
/***                                                                                                        ***/
/**************************************************************************************************************/
Service* Service::m_pInstance = nullptr;
const int Service::STATUS_CHANGED = 0;
const int Service::TIMEOUT = -1;

Service* Service::Create(const string& name, const string& description, IService *service)
{
	if(m_pInstance != nullptr) throw Service::Exception(0x0101, "Service::Create : Instance already exist");
	m_pInstance = new Service(name, description, service);
	return m_pInstance;
}

Service* Service::Get()
{
    return m_pInstance;
}

void Service::Destroy()
{
	delete m_pInstance;
	m_pInstance = nullptr;
}

Service::Service(const string& name, const string& description, IService *service) : m_Status(StatusKind::START)
{
	size_t len;

	len = name.size() + 1;
    m_pName = new char[len];
    #ifdef WIN32
    strcpy_s(m_pName, len, name.c_str());
    #else
	strcpy(m_pName, name.c_str());
    #endif

	len = description.size() + 1;
	m_pDescription = new char[len];
    #ifdef WIN32
	strcpy_s(m_pDescription, len, description.c_str());
    #else
	strcpy(m_pDescription, description.c_str());
    #endif

	m_iService = service;
	m_pInstance = this;
	m_StatusChanged.set_id(Service::STATUS_CHANGED);
	return;
}

Service::~Service()
{
    delete[] m_pName;
    delete[] m_pDescription;
	m_pInstance = nullptr;
}

void Service::SetIds(vector<reference_wrapper<ServiceConditionVariable>> cvs)
{
	for (size_t i = 0; i<cvs.size(); i++)
		cvs[i].get().set_id(i + 1);
}

int Service::Wait(vector<reference_wrapper<ServiceConditionVariable>> cvs)
{
	SetIds(cvs);
	return ServiceConditionVariable::wait();
}

int Service::WaitFor(std::vector<std::reference_wrapper<ServiceConditionVariable>> cvs, int timeout)
{
	SetIds(cvs);
	return ServiceConditionVariable::wait_for(timeout);
}

Service::StatusKind Service::GetStatus()
{
	lock_guard<mutex> lock(m_StatusAccess);
	return m_Status;
}

void Service::ChangeStatus(StatusKind status)
{
	lock_guard<mutex> lock(m_StatusAccess);
	m_Status = status;
	m_StatusChanged.notify_one();
}

#ifdef WIN32
int Service::Start(int argc, char* argv[])
{
	SERVICE_TABLE_ENTRY		dispatchTable[2];


    if (argc >= 2 && (*argv[1] == '-' || *argv[1] == '/'))
    {
        if(strcmp("install", argv[1]+1) == 0) return CmdInstall();
        if(strcmp("remove", argv[1]+1) == 0) return CmdRemove();
        if(strcmp("console", argv[1]+1) == 0) return m_iService->ServiceLoop(argc, argv);
        if(strcmp("service", argv[1]+1) == 0)
        {
            dispatchTable[0].lpServiceName = m_pName;
            dispatchTable[0].lpServiceProc = ::ServiceMain;
            dispatchTable[1].lpServiceName = NULL;
            dispatchTable[1].lpServiceProc = NULL;
			if(!StartServiceCtrlDispatcher(dispatchTable)) return m_iService->ServiceLoop(argc, argv);
			return 0;
        }
    }

	cout << endl << "Usage :" << endl;
	cout << argv[0] << " /install       To install the service" << endl;
	cout << argv[0] << " /remove        To remove the service" << endl;
	cout << argv[0] << " /console       To run as a console application" << endl;
	return -1;
}

int Service::CmdInstall()
{
    SC_HANDLE hService;
	SC_HANDLE hSCManager;
	HMODULE hLib;
	char exePath[MAX_PATH+1];
	ChangeServiceConfigType pfnChangeServiceConfig;

    //Open the service manager
	if((hSCManager=OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL)
	{
		cout << endl << "Unable to access the Service Control Manager" << endl;
		return -1;
	}

    //Create the service
	GetModuleFileName(NULL, exePath, MAX_PATH+1);
	strcat_s(exePath, MAX_PATH, " /Service");

	hService = CreateService(
				hSCManager,
				m_pName,
				m_pName,			                // Service name to display
				SERVICE_ALL_ACCESS,					// Desired access
				SERVICE_WIN32_OWN_PROCESS,			// Service type
				SERVICE_AUTO_START,					// Start type
				SERVICE_ERROR_NORMAL,				// Error control type
				exePath,			  				// Service's binary
				NULL,								// No load ordering group
				NULL,								// No tag identifier
				NULL,								// No dependencies
				NULL,								// LocalSystem account
				NULL);								// No password

	if(hService == NULL)
	{
		cout << endl << "Failed to install service " << m_pName << endl;
		return -1;
	}

	//Set the service description (only exists on W2k and up)
	if((hLib=LoadLibrary("ADVAPI32.DLL")) != NULL)
	{
		pfnChangeServiceConfig = (ChangeServiceConfigType) GetProcAddress(hLib, "ChangeServiceConfig2A");
		if(pfnChangeServiceConfig!=NULL)
		{
			SERVICE_DESCRIPTION sd;
			sd.lpDescription = m_pDescription;
			pfnChangeServiceConfig(hService, SERVICE_CONFIG_DESCRIPTION, (void*)&sd);
		}
		FreeLibrary(hLib);
	}

	//Start the service
	StartService(hService, 0, NULL);

    //Close the service manager
	CloseServiceHandle(hService);
	cout << endl << "Service " << m_pName << " installed." << endl;

	return 0;
}

int Service::CmdRemove()
{
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
	SERVICE_STATUS ServiceStatus;

    //Open the service manager
	if((hSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS)) == NULL)
	{
		cout << endl << "Unable to access the Service Control Manager" << endl;
		return -1;
	}

    //Open the service
	if((hService=OpenService(hSCManager, m_pName, SERVICE_ALL_ACCESS)) == NULL)
	{
		cout << endl << "Unable to contact the service" << m_pName << endl;
		return -1;
	}

    //Stop the service
	ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);

    //Remove the service
    if(!DeleteService(hService))
	{
	    CloseServiceHandle(hService);
		cout << endl << "Unable to remove the service" << m_pName << endl;
		return -1;
	}

    //Close the service manager
    CloseServiceHandle(hService);
    cout << endl << "Service " << m_pName << " remove" << endl;
    return 0;
}

void Service::ServiceCtrlHandler(unsigned long controlCode)
{
	switch(controlCode)
	{
		case SERVICE_CONTROL_PAUSE:
		{
			m_serviceStatus.dwCurrentState = SERVICE_PAUSED;
			ChangeStatus(StatusKind::PAUSE);
			break;
		}

		case SERVICE_CONTROL_CONTINUE:
		{
			m_serviceStatus.dwCurrentState = SERVICE_RUNNING;
			ChangeStatus(StatusKind::START);
			break;
		}

		case SERVICE_CONTROL_STOP:
		{
			m_serviceStatus.dwWin32ExitCode = 0;
			m_serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			m_serviceStatus.dwCheckPoint = 0;
			m_serviceStatus.dwWaitHint = 0;
			SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus);
			ChangeStatus(StatusKind::STOP);
			break;
		}

		case SERVICE_CONTROL_INTERROGATE:
		{
			break;
		}

		default:
		{
			break;
		}
	}

	return;
}

void Service::ServiceMain(int argc, char* argv[])
{
	m_serviceStatus.dwServiceType = SERVICE_WIN32;
	m_serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	m_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	m_serviceStatus.dwWin32ExitCode = 0;
	m_serviceStatus.dwServiceSpecificExitCode = 0;
	m_serviceStatus.dwCheckPoint = 0;
	m_serviceStatus.dwWaitHint = 0;

	m_serviceStatusHandle = RegisterServiceCtrlHandler(m_pName, ::ServiceCtrlHandler);
	if(m_serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) return;

	m_serviceStatus.dwCurrentState = SERVICE_RUNNING;
	m_serviceStatus.dwCheckPoint = 0;
	m_serviceStatus.dwWaitHint = 0;
	if(!SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus)) return;

	m_iService->ServiceLoop(argc, argv);

	m_serviceStatus.dwWin32ExitCode = 0;
	m_serviceStatus.dwCurrentState = SERVICE_STOPPED;
	m_serviceStatus.dwCheckPoint = 0;
	m_serviceStatus.dwWaitHint = 0;
	SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus);

	return;
}
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

vector<string> Service::m_runDirs = {"/var/run", "/var/adm", "/usr/adm", "~"};

int Service::FindPid()
{
    vector<string>::iterator it;
    vector<string>::const_iterator itEnd;
    char    buf[16];
    int	    pid = 0;


    itEnd = m_runDirs.end();
    for (it = m_runDirs.begin(); it != itEnd; ++it)
    {
        m_pidFile = *it+"/"+m_daemonName+".pid";
        FILE *fp = fopen(m_pidFile.c_str(), "r");
        if(fp == NULL) continue;

        if(fgets(buf, sizeof(buf), fp) != NULL) pid = atoi(buf);
        fclose(fp);
        if(pid>0) break;
    }

	return pid;
}

void Service::SetPidFile()
{
    vector<string>::iterator it;
    vector<string>::const_iterator itEnd;


    itEnd = m_runDirs.end();
    for (it = m_runDirs.begin(); it != itEnd; ++it)
    {
        if(access((*it).c_str(), R_OK | W_OK) == 0)
            break;
    }

    if (it == itEnd)
        throw Service::Exception(0x0211, "Service::FindPidFile : Unable to find a writebale directory for store pid file (/var/run, ..., or ~)");

    m_pidFile = *it+"/"+m_daemonName+".pid";
}

void Service::WritePid(pid_t pid)
{
    FILE    *fp;


    if(m_pidFile=="") SetPidFile();

    if ((fp = fopen(m_pidFile.c_str(), "w")) == NULL)
        throw Service::Exception(0x0221, "Service::WritePid : Unable to open pid file in write mode");

    fprintf(fp, "%d\n", pid);
    fclose(fp);
}

int Service::Start(int argc, char* argv[])
{
    char *pname;
    char *p;


    pname = *argv;
    if( (p = strrchr(pname, '/')) != NULL )
        m_daemonName = string(p+1);
    else
    {
        m_daemonName = string(pname);
    }

    if (argc >= 2 && (*argv[1] == '-' || *argv[1] == '/'))
    {
        if(strcmp("start", argv[1]+1) == 0) return CmdStart(argc, argv);
        if(strcmp("stop", argv[1]+1) == 0) return CmdStop();
        if(strcmp("restart", argv[1]+1) == 0) return CmdRestart(argc, argv);
        if(strcmp("console", argv[1]+1) == 0) return m_iService->ServiceLoop(argc, argv);
    }
    if (argc == 1) return m_iService->ServiceLoop(argc, argv);

	cout << endl << "Usage :" << endl;
	cout << argv[0] << " -start       To start the daemon" << endl;
	cout << argv[0] << " -stop        To stop the daemon" << endl;
	cout << argv[0] << " -restart     To restart the daemon" << endl;
	cout << argv[0] << " -console     To start on the console" << endl;
	return -1;
}

int Service::CmdStart(int argc, char* argv[])
{
    int     ret;
    pid_t   pid;


    // Fork off the parent process
    pid = fork();

    // An error occurred
    if (pid < 0) throw Service::Exception(0x0261, "Service::CmdStart : Unable to fork process");

    // Success: Let the parent terminate
    if (pid > 0) return 0;

    // On success: The child process becomes session leader
    if (setsid() < 0) throw Service::Exception(0x0262, "Service::CmdStart : Unable to set the session id");

    // Catch, ignore and handle signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // Fork off for the second time
    pid = fork();

    // An error occurred
    if (pid < 0) throw Service::Exception(0x0263, "Service::CmdStart : Unable to fork process");

    // Success: Let the parent terminate
    if (pid > 0)
    {
       	WritePid(pid);
        return 0;
    }

	signal(SIGINT, service_signal_handler);
	signal(SIGQUIT, service_signal_handler);
	signal(SIGABRT, service_signal_handler);
	signal(SIGKILL, service_signal_handler);
	signal(SIGTERM, service_signal_handler);
	signal(SIGCONT, service_signal_handler);
	signal(SIGTSTP, service_signal_handler);

    // Set new file permissions
    umask(0);

    // Change the working directory to the root directory or another appropriated directory
    chdir("/");

    // Close all open file descriptors
    //for (int i = sysconf(_SC_OPEN_MAX); i>0; i--)
    //    close (i);

    ret = m_iService->ServiceLoop(argc, argv);
    unlink(m_pidFile.c_str());
    return ret;
}

int Service::CmdStop()
{
    int pid;


    pid = FindPid();
    if(pid==0) throw Service::Exception(0x0271, "Service::CmdStop : Daemon is not started");

	if((kill(pid, SIGTERM) < 0) && (errno != ESRCH))
        throw Service::Exception(0x0271, "Service::CmdStop : Unable to kill the daemon");

	unlink(m_pidFile.c_str());
	return 0;
}

int Service::CmdRestart(int argc, char* argv[])
{
    CmdStop();
    return CmdStart(argc, argv);
}
#endif // WIN32

/**************************************************************************************************************/
/***                                                                                                        ***/
/*** Class Service::Exception                                                                               ***/
/***                                                                                                        ***/
/**************************************************************************************************************/
Service::Exception::Exception(int number, string const& message) throw() : m_number(number), m_message(message)
{
}

Service::Exception::~Exception() throw()
{
}

const char* Service::Exception::what() const throw()
{
    return m_message.c_str();
}

int Service::Exception::GetNumber() const throw()
{
    return m_number;
}
