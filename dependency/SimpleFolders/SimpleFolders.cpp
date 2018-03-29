#include <iostream>

/*** LICENCE ***************************************************************************************/
/*
  SimpleFolders - Simple class to get standards folder

  This file is part of SimpleFolders.

    SimpleFolders is free software : you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SimpleFolders is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SimpleFolders.  If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <stdexcept>
#include "SimpleFolders.h"

using namespace std;

/**************************************************************************************************************/
/***                                                                                                        ***/
/*** Class SimpleFolders                                                                                        ***/
/***                                                                                                        ***/
/**************************************************************************************************************/
string SimpleFolders::GetFolder(FolderType folderType)
{
    switch(folderType)
    {
        case Data : return GetFolderData();
        case Configuration : return GetFolderConfig();
        case Log : return GetFolderLog();
        case Exe : return GetFolderExe();
        case Temp : return GetFolderTemp();
        case User : return GetFolderUser();
        default :
            throw logic_error("SimpleFolders::GetFolder : Unknown FolderType");
    }
}

string SimpleFolders::CompleteFolder(const string& folder)
{
    ostringstream path;

    if(folder.substr(folder.length()-1)==SEPARATOR) return folder;

    path << folder << SEPARATOR;
    return path.str();
}

string SimpleFolders::AddFolder(const string& folder1, const string& folder2)
{
    ostringstream path;

    path << CompleteFolder(folder1) << folder2;
    return path.str();
}

string SimpleFolders::AddFile(const string& folder, const string& file, const string& ext)
{
    ostringstream path;

    path << CompleteFolder(folder) << file << "." << ext;
    return path.str();
}

string SimpleFolders::AddFile(const string& folder, const string& file)
{
    return AddFolder(folder, file);
}

bool SimpleFolders::FolderExists(const string& folder)
{
    struct stat info;

    if(stat(folder.c_str(), &info) != 0) return false;
    if(info.st_mode & S_IFDIR) return true;
    return false;
}

bool SimpleFolders::FileExists(const string& file)
{
    struct stat info;

    if(stat(file.c_str(), &info) != 0) return false;
    if(info.st_mode & S_IFREG) return true;
    return false;
}

#ifdef WIN32
#include <direct.h>
#include <windows.h>
typedef bool (STDMETHODCALLTYPE FAR * LPFN) (HANDLE hToken, LPTSTR lpProfileDir, LPDWORD lpcchSize);

void SimpleFolders::CreateFolder(const string& folder)
{
    ostringstream errorMsg;

    if(_mkdir(folder.c_str())!=0)
    {
        errorMsg << "Unable to create folder " << folder << " errno " << errno;
        throw logic_error(errorMsg.str().c_str());
    }
}

string SimpleFolders::GetFolderData()
{
    return GetFolderUser();
}

string SimpleFolders::GetFolderConfig()
{
    return GetFolderUser();
}

string SimpleFolders::GetFolderLog()
{
    return GetFolderUser();
}

string SimpleFolders::GetFolderTemp()
{
    DWORD   dwRetVal;
    TCHAR   folder[MAX_PATH];

    dwRetVal = GetTempPath(sizeof(folder), folder);
    if(dwRetVal > MAX_PATH || (dwRetVal == 0)) throw "SimpleFolders::GetFolderTemp : Unable to read the temp path.";

    return CompleteFolder(folder);
}

string SimpleFolders::GetFolderUser()
{
    char    folder[MAX_PATH];
    int     length;
	HMODULE hUserEnvLib = NULL;
	LPFN    GetUserProfileDirectory = NULL;
    HANDLE  hToken;


    hUserEnvLib = LoadLibrary("userenv.dll");
    if(!hUserEnvLib)
        throw "SimpleFolders::GetFolderUser : Unable to open userenv.dll.";

    GetUserProfileDirectory = (LPFN) GetProcAddress( hUserEnvLib, "GetUserProfileDirectoryA" );
    if(!GetUserProfileDirectory)
        throw "SimpleFolders::GetFolderUser : Unable to find GetUserProfileDirectory function.";

    if((OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) == 0)
        throw "SimpleFolders::GetFolderUser : Unable to get current process.";

    length = sizeof(folder);
    if(((GetUserProfileDirectory(hToken, folder, (LPDWORD) &length)) == 0) && (GetLastError()!=0))
    {
		CloseHandle(hToken);
		throw "SimpleFolders::GetFolderUser : Unable to get user profile directory.";
    }

    CloseHandle(hToken);

    return CompleteFolder(folder);
}

string SimpleFolders::GetFolderExe()
{
	char    folder[MAX_PATH];
	char    *p;

	GetModuleFileName(NULL, folder, MAX_PATH);
	p = strrchr(folder, '\\');
	if(p == NULL)
        throw "SimpleFolders::GetFolderExe : Unable to find exe directory.";

    *p = '\0';
    return CompleteFolder(folder);
}
#else
#include <errno.h>
#include <unistd.h>
#include <pwd.h>

string SimpleFolders::GetFolderData()
{
    return "/var/lib/";
}

string SimpleFolders::GetFolderConfig()
{
    return "/usr/local/etc/";
}

string SimpleFolders::GetFolderLog()
{
    return "/var/log/";
}

string SimpleFolders::GetFolderTemp()
{
    return "/var/tmp/";
}

string SimpleFolders::GetFolderUser()
{
    struct passwd *pw = getpwuid(getuid());

    return CompleteFolder(pw->pw_dir);
}

string SimpleFolders::GetFolderExe()
{
    return "/proc/self/exe/";
}

void SimpleFolders::CreateFolder(const string& folder)
{
    ostringstream errorMsg;

    if(mkdir(folder.c_str(), 0755)!=0)
    {
        errorMsg << "Unable to create folder " << folder << " errno " << errno;
        throw logic_error(errorMsg.str().c_str());
    }
}
#endif
