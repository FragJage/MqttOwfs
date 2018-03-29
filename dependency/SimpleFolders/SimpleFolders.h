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

/*** MAIN PAGE FOR DOXYGEN *************************************************************************/
/// \mainpage SimpleFolders Class Documentation
/// \section intro_sec Introduction
///
/// This class allows you to easily get standards folders such as data, configuration, log...\n
/// To use, include in your project SimpleFolders.cpp and SimpleFolders.h.
///
/// \section feature_sec Features
///
/// \li Get appdata folder
/// \li Get configuration files folder
/// \li Get log folder
/// \li Compile on Linux and Windows, Intel or ARM.
///
/// \section portability_sec Portability
/// Unit tests passed successfully on :
/// \li Windows Seven (CPU Intel Celeron)
/// \li Linux Ubuntu (CPU Intel Atom)
/// \li Linux Raspian on Raspberry Pi (CPU ARM)
/// \li Linux FunPlug on NAS DNS-320 (CPU ARM)\n
///
/// \section example_sec Example
/// \code
/// #include <iostream>
/// #include "SimpleFolders.h"
///
/// using namespace std;
///
/// int main()
/// {
///     SimpleFolders ini;
///
///     return 0;
/// }
/// \endcode
///
/// \section licence_sec Licence
///  SimpleFolders is free software : you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\n
///  SimpleFolders is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\n
///  You should have received a copy of the GNU General Public License along with SimpleFolders. If not, see <http://www.gnu.org/licenses/>.
///
/***************************************************************************************************/

#ifndef SIMPLEFOLDERS_H
#define SIMPLEFOLDERS_H

#ifdef WIN32
    #include <windows.h>
    #define SEPARATOR "\\"
#else
    #define SEPARATOR "/"
#endif
#include <string>

/// \brief    Very simple class to get standards folders name
/// \details  Class allows you to easily get standards folders name.
class SimpleFolders
{
    public:
        enum FolderType {Data, Configuration, Log, Exe, Temp, User};

        /// \brief    Get a folder
        /// \details  Get a specific folder
        /// \return   Return the folder name.
        static std::string GetFolder(FolderType folderType);

        static std::string CompleteFolder(const std::string& folder);
        static std::string AddFolder(const std::string& folder1, const std::string& folder2);
        static std::string AddFile(const std::string& folder, const std::string& file, const std::string& ext);
        static std::string AddFile(const std::string& folder, const std::string& file);

        static bool FolderExists(const std::string& folder);
        static bool FileExists(const std::string& file);
        static void CreateFolder(const std::string& folder);

    private:
        static std::string GetFolderData();
        static std::string GetFolderConfig();
        static std::string GetFolderLog();
        static std::string GetFolderExe();
        static std::string GetFolderTemp();
        static std::string GetFolderUser();
};

#endif // SIMPLEFOLDERS_H
