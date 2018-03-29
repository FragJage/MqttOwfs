/*** LICENCE ***************************************************************************************/
/*
  owfscpp - Simple class to communicate with owserver, the backend component of the OWFS 1-wire bus control system.

  This file is part of owfscpp.

    owfscpp is free software : you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    owfscpp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with owfscpp. If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************************/

/*** MAIN PAGE FOR DOXYGEN *************************************************************************/
/// \mainpage OwfsCpp Class Documentation
///
/// \section intro_sec Introduction
/// A simple class to communicate with owserver, the backend component of the OWFS 1-wire bus control system.\n
/// See http://owfs.org/ for more information on OWFS 1-wire bus control system.\n
/// To use, include in your project owfscpp.cpp, owfscpp.h, SimpleSock.cpp, SimpleSock.h, SafeFunctions.cpp and SafeFunctions.h.
/// \author Francois GELLEZ
/// \version 1.0
/// \date 16/10/2015
///
/// \section feature_sec Features
/// \li All messages of owserver protocol are supported
/// \li Support of persitentes connections
/// \li All owserver flags are modifiable
/// \li Compile on Linux and Windows, Intel or ARM.
///
/// \section quality_sec Quality developpment
/// \li Static audit with CppCheck
/// \li Security audit with VisualCodeGrepper
/// \li Dynamic audit with Address Sanitizer.
/// \li Unit tests replay for each version.
///
/// \section portability_sec Portability
/// Unit tests passed successfully on :
/// \li Windows Seven (CPU Intel Celeron)
/// \li Linux Ubuntu (CPU Intel Atom)
/// \li Linux Raspian on Raspberry Pi (CPU ARM)
/// \li Linux FunPlug on NAS DNS-320 (CPU ARM)
/// \n(Compilation directives define LINUX or WIN only necessary for colours in unit tests)
///
/// \section example_sec Example
/// \code
/// #include <iostream>
/// #include "owfscpp.h"
///
/// using namespace std;
///
/// int main()
/// {
///     owfscpp owfsClient;
///     list<string> lstDir;
///     list<string>::iterator iDir;
///
///     owfsClient.Initialisation("192.168.0.20", 4304);
///
///     lstDir = owfsClient.DirAll("/");
///     for(iDir = lstDir.begin(); iDir != lstDir.end(); ++iDir)
///     {
///         cout << *iDir << endl;
///     }
///
///     return 0;
/// }
/// \endcode
///
/// \section WhatsNew1_sec What's New in version 1.0
/// \li All owserver flags are modifiable
/// \li Improving the portability
/// \li Add audit of source code
///
/// \section licence_sec Licence
///  OwfsCpp is free software : you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\n
///  OwfsCpp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\n
///  You should have received a copy of the GNU General Public License along with OwfsCpp. If not, see <http://www.gnu.org/licenses/>.
///
/***************************************************************************************************/

#ifndef owfscpp_H
#define owfscpp_H

#include <list>
#include "SimpleSock/SimpleSockTCP.h"

/// \brief    Simple cpp class 1-wire interface
/// \details  Simple class to communicate with owserver, the backend component of the OWFS 1-wire bus control system.
class owfscpp
{
public:
    /// FormatDevice enum, different format device display name
    enum FormatDevice
    {
        ///Famille.Identifier
        FaPtId,
        ///FamilleIdentifier
        FaId,
        ///Famille.Identifier.Code
        FaPtIdPtCo,
        ///Famille.IdentifierCode
        FaPtIdCo,
        ///FamilleIdentifier.Code
        FaIdPtCo,
        ///FamilleIdentifierCode
        FaIdCo
    };

    /// TemperatureScale enum, different scale of temperature
    enum TemperatureScale
    {
        ///Centigrade
        Centigrade,
        ///Fahrenheit
        Fahrenheit,
        ///Kelvin
        Kelvin,
        ///Rankine
        Rankine
    };

    /// PressureScale enum, different scale of pressure
    enum PressureScale
    {
        ///millibar
        Mbar,
        ///atmosphere
        Atm,
        ///millimeter of mercury
        MmHg,
        ///inch of mercury
        InHg,
        ///pound-force per square inch
        Psi,
        ///pascal
        Pa
    };

    /// OwserverFlag enum, different flag option
    enum OwserverFlag
    {
        ///Implicit or uncached read
        Uncached,
        ///Restrict operations to reads and cached, more
        Safemode,
        ///Use aliases for known slaves (human readable names)
        Alias,
        ///Request and/or Grant persistence
        Persistence,
        ///Include special directories (settings, statistics, uncached,...)
        Bus_ret
    };

    class Exception;

    /// \brief    Constructor of owfscpp
    /// \param    server         IP address of owserver.
    /// \param    port           Port TCP address of owserver.
    /// \details  Constructor of owfscpp.
    owfscpp(std::string server="", unsigned port=0);

    /// \brief    Destructor of owfscpp
    /// \details  Destructor of owfscpp.
    ~owfscpp();

    /// \brief    Define owserver connexion
    /// \param    server         IP address of owserver.
    /// \param    port           Port TCP address of owserver.
    /// \details  Define IP address and TCP port of owserver.
    void Initialisation(std::string server, unsigned port);

    /// \brief    Enable/Disable owserver flag
    /// \param    flag           enum Owserver Flag
    /// \param    active         TRUE or FALSE.
    /// \details  Enable or disable different owserver flag such as persistence.
    void SetOwserverFlag(OwserverFlag flag, bool active);

    /// \brief    Get owserver flags
    /// \details  Return unsigned int representing owserver flags.
    unsigned GetOwserverFlags();

    /// \brief    Set the device display
    /// \param    format        enum format display.
    /// \details  Set the format device display name
    void SetDeviceDisplay(FormatDevice format);

    /// \brief    Set the temperature scale
    /// \param    scale        enum temperature scale
    /// \details  Set the temperature scale
    void SetTemperatureScale(TemperatureScale scale);

    /// \brief    Set the pressure scale
    /// \param    scale        enum pressure scale
    /// \details  Set the pressure scale
    void SetPressureScale(PressureScale scale);

    /// \brief    Dir function of owserver
    /// \param    path         Path of the ressource.
    /// \details  List 1-wire bus
    std::list<std::string> Dir(const std::string& path);

    /// \brief    DirAll function of owserver
    /// \param    path         Path of the ressource.
    /// \details  List 1-wire bus, faster, but consumes more memory than Dir function
    std::list<std::string> DirAll(const std::string& path);

    /// \brief    DirAllSlash function of owserver
    /// \param    path         Path of the ressource.
    /// \details  List 1-wire bus, dirall but with directory entries getting a trailing '/'
    std::list<std::string> DirAllSlash(const std::string& path);

    /// \brief    Present function of owserver
    /// \param    path         Path of the ressource.
    /// \details  Return true if the specified component recognized and known
    bool Present(const std::string& path);

    /// \brief    Read function of owserver
    /// \param    path         Path of the ressource.
    /// \details  Read from 1-wire bus
    std::string Read(const std::string& path);

    /// \brief    Write function of owserver
    /// \param    path         Path of the ressource.
    /// \param    value        Value to write.
    /// \details  Write to 1-wire bus
    void Write(const std::string& path, const std::string& value);

    /// \brief    Get function of owserver
    /// \param    path         Path of the ressource.
    /// \details  Dirall or read depending on path
    std::string Get(const std::string& path);

    /// \brief    GetSlash function of owserver
    /// \param    path         Path of the ressource.
    /// \details  DirAllSlash or read depending on path
    std::string GetSlash(const std::string& path);

private:
    std::string ReadSkip85(const std::string& path, bool skipBug85);
    std::string GetSkip85(const std::string& path, bool skipBug85);
    std::string SkipBug85(const std::string& path, const std::string& value, bool isGet);
    void Connect();
    void Close();
    void WaitRecv();
    char* WriteInt32(char* buffer, unsigned value);
    int ReadInt32(char* buffer, unsigned pos);
    std::string RecvBlock();
    void SendMessage(unsigned message, const std::string& path, const std::string& value);
    void SetPersistence(bool persistence);

    std::string m_OwfsServer;
    unsigned m_OwfsPort;
    unsigned m_Timeout;
    unsigned m_Version;
    unsigned m_ControlFlags;
    bool m_Persistence;
    int m_PersistenceMode;
    SimpleSockTCP m_OwfsSock;

    static const unsigned OWFSMSG_READ = 2;
    static const unsigned OWFSMSG_WRITE = 3;
    static const unsigned OWFSMSG_DIR = 4;
    static const unsigned OWFSMSG_PRESENT = 6;
    static const unsigned OWFSMSG_DIRALL = 7;
    static const unsigned OWFSMSG_GET = 8;
    static const unsigned OWFSMSG_DIRALLSLASH = 9;
    static const unsigned OWFSMSG_GETSLASH = 10;

    static const int PERSISTENCE_OPEN = 1;
    static const int PERSISTENCE_CLOSE = 2;
    static const int PERSISTENCE_RETRY = 3;
};

/// \brief    Exception management
/// \details  Class to manage exception on owfscpp.
class owfscpp::Exception: public std::exception
{
public:
    /// \brief    Constructor of owfscpp::Exception
    /// \details  Constructor of owfscpp::Exception personalized exception for owfscpp.
    Exception(int number, std::string const& message, int system=0) throw();

    /// \brief    Destructor of owfscpp::Exception
    /// \details  Destructor of owfscpp::Exception personalized exception for owfscpp.
    ~Exception() throw();

    /// \brief    Get string identifying exception
    /// \details  Returns a null terminated character sequence that may be used to identify the exception.
    const char* what() const throw();

    /// \brief    Get the number of exception
    /// \details  Returns a number to identify the exception.
    int GetNumber() const throw();

    /// \brief    Get the owserver error number.
    /// \details  Returns error number send by owserver.
    int GetOwfsError() const throw();

private:
    void SetWhatMsg();
    int m_number;
    std::string m_message;
    int m_system;
    std::string m_whatMsg;
};
#endif // owfscpp_H
