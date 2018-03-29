/*** LICENCE ***************************************************************************************/
/*
  SimpleSock - Simple class to manage socket communication TCP or UDP

  This file is part of SimpleSock.

    SimpleSock is free software : you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SimpleSock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SimpleSock.  If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************************/

/*** MAIN PAGE FOR DOXYGEN *************************************************************************/
/// \mainpage SimpleSock Class Documentation
///
/// \section intro_sec Introduction
/// Very simple class to manage socket communication TCP or UDP.\n
/// To use, include in your project SimpleSock.cpp, SimpleSock.h, SafeFunctions.cpp and SafeFunctions.h.
/// \author Francois GELLEZ
/// \version 1.1
/// \date 13/10/2015
///
/// \section feature_sec Features
/// \li Support TCP and UDP Socket
/// \li Calculate broadcast address
/// \li Family AF_INET (IPv4)
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
/// \li Windows Seven (CPU Intel Celeron) with MinGW
/// \li Linux Ubuntu (CPU Intel Atom)
/// \li Linux Raspian on Raspberry Pi (CPU ARM)
/// \li Linux FunPlug on NAS DNS-320 (CPU ARM)
/// \n For GCC versions < 4.7 replaced -std=C++11 by -std=C++0x
///
/// \section example_sec Example
/// \code
/// #include <iostream>
/// #include "SimpleSock.h"
///
/// using namespace std;
///
/// int main()
/// {
///     string buffer;
///
///     SimpleSockUDP sockUDPemetteur;
///     SimpleSockUDP sockUDPrecepteur;
///
///     sockUDPemetteur.Open(4639, "127.0.0.1");
///     sockUDPrecepteur.Listen(4639, "127.0.0.1");
///     sockUDPrecepteur.Blocking(false);
///
///     cout << "Emit : TOTO-test" << endl;
///     sockUDPemetteur.Send("TOTO-test");
///     sockUDPrecepteur.Recv(buffer);
///     cout << "Receive : " << buffer << endl;
///
///     sockUDPrecepteur.Close();
///     sockUDPemetteur.Close();
///
///     return 0;
/// }
/// \endcode
///
/// \section WhatsNew1_sec What's New in version 1.1
/// \li Add security audit of source code
/// \li Add many Warning Options on GCC
///
/// \section licence_sec Licence
///  SimpleSock is free software : you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\n
///  SimpleSock is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.\n\n
///  You should have received a copy of the GNU General Public License along with SimpleSock. If not, see <http://www.gnu.org/licenses/>.
///
/***************************************************************************************************/

#ifndef SIMPLESOCK_H
#define SIMPLESOCK_H

#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <exception>
#include "SafeFunctions/SafeFunctions.h"

#ifdef __linux__
    #include <cerrno>
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <net/if.h>
    #include <fcntl.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET int
    #define ARG_ACCEPT socklen_t
    #define closesocket(s) close(s)
#else
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define ARG_ACCEPT int
#endif

class ISimpleSock
{
  public:
    virtual void SetSocket(SOCKET sockHandle, struct sockaddr_in sockAddress, int type) = 0;
    virtual void Open(int port, const std::string& ipAddress) = 0;
    virtual void Open(int port, unsigned long ipAddress) = 0;
    virtual bool isOpen() = 0;
    virtual void Close() = 0;
    virtual void Listen(int port, const std::string& ipAddress) = 0;
    virtual void Listen(int port, unsigned long ipAddress) = 0;
    virtual bool WaitRecv(int delay) = 0;
    virtual void Send(const std::string& buffer) = 0;
    virtual void Send(const char* buffer, size_t bufferSize) = 0;
    virtual bool Recv(std::string& buffer, int sizeMax=-1) = 0;
    virtual unsigned Recv(char* buffer, size_t bufferSize) = 0;
    virtual void Blocking(bool blocking) = 0;
    virtual int GetPort() = 0;
    virtual std::string GetAddress() = 0;
};

/// \brief    Very simple class to manage sockets with very few methods.
/// \details  This class provides the basis for SimpleSockUDP & SimpleSockTCP class.
class SimpleSock : public ISimpleSock
{
    public:
        class Exception;

        /// \brief    Constructor for accepted socket
        /// \param    sockHandle    Handle of the socket.
        /// \param    sockAddress   Address of the connecting entity.
        /// \param    type          Type specification for the socket.
        /// \details  Constructor of SimpleSock for accepted socket (See SimpleSockTCP::Accept).
        void SetSocket(SOCKET sockHandle, struct sockaddr_in sockAddress, int type);

        /// \brief    Open a connexion
        /// \param    port          Port number.
        /// \param    ipAddress     IP Address of the host.
        /// \details  Open a connexion to the IP Address "ipAddress" on the port "port".
        void Open(int port, const std::string& ipAddress);

        /// \brief    Open a connexion
        /// \param    port          Port number.
        /// \param    ipAddress     IP Address of the host.
        /// \details  Open a connexion to the IP Address "ipAddress" on the port "port".
        void Open(int port, unsigned long ipAddress);

        /// \brief    To test if the connection is open
        /// \details  Return true if the connection is open.
        bool isOpen();

        /// \brief    Close the connection
        /// \details  Shutdown and close the connection.
        void Close();

        /// \brief    Listen for incoming connections
        /// \param    port          Port number.
        /// \param    ipAddress     IP Address of the host.
        /// \details  Listen for incoming connections IP Address "ipAddress" on the port "port".
        void Listen(int port, const std::string& ipAddress);

        /// \brief    Listen for incoming connections
        /// \param    port          Port number.
        /// \param    ipAddress     IP Address of the host.
        /// \details  Listen for incoming connections IP Address "ipAddress" on the port "port".
        void Listen(int port, unsigned long ipAddress);

        /// \brief    Wait for receive data
        /// \param    delay         Delay in microseconds.
        /// \details  Return true if data can be receive.
        bool WaitRecv(int delay);

        /// \brief    Send a string
        /// \param    buffer         String to send
        /// \details  Send a string to the host.
        void Send(const std::string& buffer);

        /// \brief    Send a buffer
        /// \param    buffer         Buffer to send
        /// \param    bufferSize         Size of the buffer
        /// \details  Send a buffer to the host.
        void Send(const char* buffer, size_t bufferSize);

        /// \brief    Receive a string
        /// \param    buffer         String to receive
        /// \param    sizeMax      Maximum size to receive
        /// \details  Receive a string from the host.
        bool Recv(std::string& buffer, int sizeMax=-1);

        /// \brief    Receive a buffer
        /// \param    buffer      Buffer to receive
        /// \param    bufferSize         Size of the buffer
        /// \details  Receive a buffer from the host.
        unsigned Recv(char* buffer, size_t bufferSize);

        /// \brief    Set the blocking mode
        /// \param    blocking    True or false
        /// \details  Set the blocking mode.
        void Blocking(bool blocking);

        /// \brief    Get the port
        /// \details  Get the port of the connection
        int GetPort();

        /// \brief    Get the IP Address
        /// \details  Get the IP Address of the connection
        std::string GetAddress();

    protected:
        /// \brief    Constructor of SimpleSock
        /// \param    type         The type specification for the new socket.
        /// \details  Constructor of SimpleSock, type is SOCK_STREAM or SOCK_DGRAM.
        SimpleSock(int type);

        /// \brief    Destructor of SimpleSock
        /// \details  Destructor of SimpleSock.
        virtual ~SimpleSock();

        /// \brief    Get the last SocketError
        /// \details  Get the last SocketError.
        int GetSocketError();

        /// \brief    Address
        /// \details  Address of the connecting entity.
        struct sockaddr_in m_sockAddress;

        /// \brief    Socket handle
        /// \details  Socket handle for internal OS function
        SOCKET m_sockHandle;

        /// \brief    To test if the connection is open
        /// \details  True if the connection is open.
        bool m_isOpen;

    private:
        void InitSocket();
        bool OperationOk();
        static bool m_initSocket;
        int m_sockType;
        bool m_blocking;
};

/// \brief    Exception management
/// \details  Class to manage exception on SimpleSock.
class SimpleSock::Exception: public std::exception
{
public:
    /// \brief    Constructor of SimpleSock::Exception
    /// \details  Constructor of SimpleSock::Exception personalized exception for SimpleSock.
    Exception(int number, std::string const& message) throw();

    /// \brief    Constructor of SimpleSock::Exception
    /// \details  Constructor of SimpleSock::Exception personalized exception for SimpleSock.
    Exception(int number, std::string const& message, int system) throw();

    /// \brief    Destructor of SimpleSock::Exception
    /// \details  Destructor of SimpleSock::Exception personalized exception for SimpleSock.
    ~Exception() throw();

    /// \brief    Get string identifying exception
    /// \details  Returns a null terminated character sequence that may be used to identify the exception.
    const char* what() const throw();

    /// \brief    Get the number of exception
    /// \details  Returns a number to identify the exception.
    int GetNumber() const throw();

    private:
        void SetWhatMsg();
        int m_number;
        std::string m_message;
        int m_system;
        std::string m_whatMsg;
};

#endif // SIMPLESOCK_H
