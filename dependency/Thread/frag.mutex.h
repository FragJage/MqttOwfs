#ifndef WIN32STDMUTEX_H
#define WIN32STDMUTEX_H
#ifdef _GLIBCXX_HAS_GTHREADS
#error This version of MinGW seems to include a win32 port of pthreads, and probably    \
    already has C++11 std threading classes implemented, based on pthreads.             \
    You are likely to have class redefinition errors below, and unfirtunately this      \
    implementation can not be used standalone                                           \
    and independent of the system <mutex> header, since it relies on it for             \
    std::unique_lock and other utility classes. If you would still like to use this     \
    implementation (as it is more lightweight), you have to edit the                    \
    c++-config.h system header of your MinGW to not define _GLIBCXX_HAS_GTHREADS.       \
    This will prevent system headers from defining actual threading classes while still \
    defining the necessary utility classes.
#endif

#include <windows.h>

namespace std
{
class mutex
{
public:
    mutex()
    {
        m_Mutex = CreateMutex(NULL, FALSE, NULL);
    }

    ~mutex()
    {
        CloseHandle(m_Mutex);
    }

    void lock()
    {
        WaitForSingleObject(m_Mutex, INFINITE);
    }
    void unlock()
    {
        ReleaseMutex(m_Mutex);
    }
    bool try_lock()
    {
        return (WaitForSingleObject(m_Mutex, 0)==0);
    }
private:
    HANDLE m_Mutex;
};
}
#endif // WIN32STDMUTEX_H
