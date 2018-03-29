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

#ifndef SIMPLESOCKUDP_H
#define SIMPLESOCKUDP_H

#include "SimpleSock.h"

class ISimpleSockUDP : public ISimpleSock
{
  public:
    virtual void Open(int port) = 0;
    virtual void Listen(int port, const std::string& ipAddress) = 0;
    virtual void Listen(int port, unsigned long ipAddress) = 0;
    virtual void Listen(int port) = 0;
    virtual unsigned long BroadcastAddress(const std::string& interfaceName) = 0;
    virtual std::string LocalAddress(const std::string& interfaceName) = 0;
    virtual void SetNetworkInterface(const std::string& networkInterface) = 0;
};

/// \brief    Very simple class to manage UDP sockets
/// \details  Class allows you to easily manage UDP sockets with very few methods.
class SimpleSockUDP : public SimpleSock,  public ISimpleSockUDP
{
    public:
        /// \brief    Constructor of SimpleSockUDP
        /// \details  Constructor of SimpleSockUDP.
        SimpleSockUDP();

        /// \brief    Open a connexion
        /// \param    port          Port number.
        /// \details  Open a connexion on the port "port".
        void Open(int port);

        /// \brief    Listen for incoming connections
        /// \param    port          Port number.
        /// \details  Listen for incoming connections on the port "port".
        void Listen(int port);

        /// \brief    Calculates the broadcast address
        /// \param    interfaceName     Interface name (eth0 generaly for linux, IP Address for Windows)
        /// \details  Calculates the broadcast address.
        unsigned long BroadcastAddress(const std::string& interfaceName);

        /// \brief    Get the local address
        /// \param    interfaceName     Interface name (eth0 generaly for linux, IP Address for Windows)
        /// \details  Return the local address.
        std::string LocalAddress(const std::string& interfaceName);

        /// \brief    Get the local address
        /// \param    interfaceName     Interface name (eth0 generaly for linux, IP Address for Windows)
        /// \details  Set the network interface for broadcast address calculation.
        void SetNetworkInterface(const std::string& networkInterface);

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

    private:
        bool GetInterfaceInfo(const std::string& InterfaceName, unsigned int Socket, struct in_addr *localAddr, struct in_addr *maskAddr);
        std::string m_networkInterface;
};

#endif // SIMPLESOCKUDP_H
