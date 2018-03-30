#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <assert.h>
#include "Service.h"
#include "SafeFunctions/SafeFunctions.h"

#ifdef _MSC_VER
#pragma warning( disable : 4996)
#endif

using namespace std;

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
#endif

/**************************************************************************************************************/
/***                                                                                                        ***/
/*** Class Service                                                                                          ***/
/***                                                                                                        ***/
/**************************************************************************************************************/
Service* Service::m_pInstance = nullptr;

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

Service::Service(const string& name, const string& description, IService *service)
{
    m_pName = new char[name.size()+1];
    strcpy(m_pName, name.c_str());
	m_pDescription = new char[description.size()+1];
	strcpy(m_pDescription, description.c_str());
	m_iService = service;
	m_pInstance = this;
	return;
}

Service::~Service()
{
    delete[] m_pName;
    delete[] m_pDescription;
	m_pInstance = nullptr;
}

#ifdef WIN32
int Service::Start(int argc, char* argv[])
{
	SERVICE_TABLE_ENTRY		dispatchTable[2];


    if (argc >= 2 && (*argv[1] == '-' || *argv[1] == '/'))
    {
        if(strcmp("Install", argv[1]+1) == 0) return CmdInstall();
        if(strcmp("Remove", argv[1]+1) == 0) return CmdRemove();
        if(strcmp("Console", argv[1]+1) == 0) return m_iService->ServiceStart(argc, argv);
        if(strcmp("Service", argv[1]+1) == 0)
        {
            dispatchTable[0].lpServiceName = m_pName;
            dispatchTable[0].lpServiceProc = ::ServiceMain;
            dispatchTable[1].lpServiceName = NULL;
            dispatchTable[1].lpServiceProc = NULL;
			if(!StartServiceCtrlDispatcher(dispatchTable)) return m_iService->ServiceStart(argc, argv);
			return 0;
        }
    }

	cout << endl << "Usage :" << endl;
	cout << argv[0] << " /Install       To install the service" << endl;
	cout << argv[0] << " /Remove        To remove the service" << endl;
	cout << argv[0] << " /Console       To run as a console application" << endl;
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
	strcat(exePath, " /Service");

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
			m_iService->ServicePause(true);
			break;
		}

		case SERVICE_CONTROL_CONTINUE:
		{
			m_serviceStatus.dwCurrentState = SERVICE_RUNNING;
			m_iService->ServicePause(false);
			break;
		}

		case SERVICE_CONTROL_STOP:
		{
			m_serviceStatus.dwWin32ExitCode = 0;
			m_serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			m_serviceStatus.dwCheckPoint = 0;
			m_serviceStatus.dwWaitHint = 0;
			SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus);
			m_iService->ServiceStop();
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

	m_iService->ServiceStart(argc, argv);

	m_serviceStatus.dwWin32ExitCode = 0;
	m_serviceStatus.dwCurrentState = SERVICE_STOPPED;
	m_serviceStatus.dwCheckPoint = 0;
	m_serviceStatus.dwWaitHint = 0;
	SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus);

	return;
}
#else
#include <unistd.h>
#include <signal.h>
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
        if(strcmp("console", argv[1]+1) == 0) return m_iService->ServiceStart(argc, argv);
    }
    if (argc == 1) return m_iService->ServiceStart(argc, argv);

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
    //TODO: Implement a working signal handler
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

    // Set new file permissions
    umask(0);

    // Change the working directory to the root directory or another appropriated directory
    chdir("/");

    // Close all open file descriptors
    //for (int i = sysconf(_SC_OPEN_MAX); i>0; i--)
    //    close (i);

    ret = m_iService->ServiceStart(argc, argv);
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
