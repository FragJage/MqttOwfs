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

#ifndef SIMPLESOCKTCP_H
#define SIMPLESOCKTCP_H

#include "SimpleSock.h"

class ISimpleSockTCP : public ISimpleSock
{
  public:
    virtual void Connect(const std::string& ipAddress, int port) = 0;
    virtual bool Accept(ISimpleSockTCP* sockAccept) = 0;
    virtual void Listen(int port) = 0;
};

/// \brief    Very simple class to manage TCP sockets
/// \details  Class allows you to easily manage TCP sockets with very few methods.
class SimpleSockTCP : public SimpleSock,  public ISimpleSockTCP
{
    public:
        /// \brief    Constructor of SimpleSockTCP
        /// \details  Constructor of SimpleSockTCP, optionally can load configuration file \a filename, by Load method. If the Load method fails, an exception is raised.
        SimpleSockTCP();

        /// \brief    Open a connexion
        /// \param    port          Port number.
        /// \param    ipAddress     IP Address of the host.
        /// \details  Open a connexion to the IP Address "ipAddress" on the port "port".
        void Connect(const std::string& ipAddress, int port);

        /// \brief    Accept an incoming connections
        /// \param    sockAccept    Socket of the incoming connection.
        /// \details  Accept an incoming connection and set the sockAccept.
        bool Accept(ISimpleSockTCP* sockAccept);

        /// \brief    Listen for incoming connections
        /// \param    port          Port number.
        /// \details  Listen for incoming connections on the port "port".
        void Listen(int port);

        void SetSocket(SOCKET sockHandle, struct sockaddr_in sockAddress, int type);
        void Open(int port, const std::string& ipAddress);
        void Open(int port, unsigned long ipAddress);
        bool isOpen();
        void Close();
        void Listen(int port, const std::string& ipAddress);
        void Listen(int port, unsigned long ipAddress);
        bool WaitRecv(int delay);
        void Send(const std::string& buffer);
        void Send(const char* buffer, size_t bufferSize);
        bool Recv(std::string& buffer, int sizeMax=-1);
        unsigned Recv(char* buffer, size_t bufferSize);
        void Blocking(bool blocking);
        int GetPort();
        std::string GetAddress();
};

#endif // SIMPLESOCKTCP_H
