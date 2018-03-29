#include "Plateforms.h"
#include <cstdarg>

using namespace std;

#ifdef _WIN32

void Plateforms::delay(unsigned long ms)
{
    Sleep( ms );
}

pid_t Process::launch(string command)
{
    PROCESS_INFORMATION process;
    STARTUPINFO startup;

    memset(&startup, 0, sizeof(startup));
    startup.cb = sizeof(startup);
    CreateProcessA(0, const_cast<LPSTR>(command.c_str()), 0, 0, TRUE, 0, 0, 0, &startup, &process);
    return process;
}

bool Process::terminate(pid_t pid)
{
    BOOL result;

    result = TerminateProcess(pid.hProcess, 0);
    CloseHandle(pid.hProcess);
    CloseHandle(pid.hThread);

    return result;
}

bool Process::exist(pid_t pid)
{
    DWORD result;

    result = WaitForSingleObject(pid.hProcess, 0);
    return result == WAIT_TIMEOUT;
}

#else

#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include<cstdlib>
#include<csignal>
void Plateforms::delay(unsigned long ms)
{
    usleep(ms*1000);
}

pid_t Process::launch(string command)
{
    pid_t pid;

    pid = fork();
    if(pid==0)
    {
        execl(command.c_str(), command.c_str(), nullptr);
        exit(1);
    }
    return pid;
}

bool Process::terminate(pid_t pid) //, bool forceKill)
{
    int signal;


    //if(forceKill==true)
    //    signal = SIGKILL;
    //else
        signal = SIGTERM;

    if(kill(pid, signal) == 0) return true;
    if (errno == ESRCH) return true;
    return false;
}

bool Process::exist(pid_t pid)
{
    while(waitpid(-1, 0, WNOHANG) > 0);     // Wait for defunct....
    if(kill(pid, 0) == 0) return true;        // Process exists
    return false;
}


#endif
