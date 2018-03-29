#ifndef Plateforms_H
#define Plateforms_H

#include<string>

#ifdef _WIN32
#include <windows.h>
#define pid_t PROCESS_INFORMATION
#define _NO_OLDNAMES
#endif // _WIN32

class Plateforms
{
    public:
        static void delay(unsigned long ms);
};

class Process
{
    public:
        static pid_t launch(std::string command);
        static bool terminate(pid_t pid);
        static bool exist(pid_t pid);
};
#endif
