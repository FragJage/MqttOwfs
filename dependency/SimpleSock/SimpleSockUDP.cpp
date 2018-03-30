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
#include "SimpleSockUDP.h"
using namespace std;

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSockUDP                                                                         ***/
/***                                                                                             ***/
/***************************************************************************************************/
SimpleSockUDP::SimpleSockUDP() : SimpleSock(SOCK_DGRAM), m_networkInterface("")
{
}

void SimpleSockUDP::SetNetworkInterface(const string& networkInterface)
{
    m_networkInterface = networkInterface;
}

void SimpleSockUDP::Open(int port)
{
    SimpleSock::Open(port, BroadcastAddress(m_networkInterface));
    //Open(port, INADDR_BROADCAST); //-> Erreur sur le sendto
}

void SimpleSockUDP::Listen(int port)
{
    SimpleSock::Listen(port, BroadcastAddress(m_networkInterface));
}

string SimpleSockUDP::LocalAddress(const string& interfaceName="")
{
    int sockUDP;
    int arg = 1;
    struct in_addr netmaskAddr;
    struct in_addr interfaceAddr;
    bool bOK;


	if((sockUDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1) return "127.0.0.1";
	bOK = ((setsockopt(sockUDP, SOL_SOCKET, SO_BROADCAST, (char*)&arg, sizeof(int))) != -1);
	if(bOK)
	{
        bOK = GetInterfaceInfo(interfaceName, sockUDP, &interfaceAddr, &netmaskAddr);
    }

    closesocket(sockUDP);
	if (bOK)
	{
		char str[INET_ADDRSTRLEN];
		return string(inet_ntop(AF_INET, &interfaceAddr, str, INET_ADDRSTRLEN)); //inet_ntoa(interfaceAddr);
	}
    return "127.0.0.1";
}

unsigned long SimpleSockUDP::BroadcastAddress(const string& interfaceName="")
{
    SOCKET sockUDP;
    int arg = 1;
	unsigned long addr; //= inet_addr("127.255.255.255");
    struct in_addr netmaskAddr;
    struct in_addr interfaceAddr;


	inet_pton(AF_INET, "127.255.255.255", &addr);

	if((sockUDP = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) return addr;
	if((setsockopt(sockUDP, SOL_SOCKET, SO_BROADCAST, (char*)&arg, sizeof(int))) != -1)
	{
        if(GetInterfaceInfo(interfaceName, sockUDP, &interfaceAddr, &netmaskAddr))
            addr = (~netmaskAddr.s_addr)|interfaceAddr.s_addr;
    }

    closesocket(sockUDP);
    return addr;
}

#ifdef __linux__
#pragma GCC diagnostic push         //I don't understand, perhaps a false positive
#pragma GCC diagnostic ignored "-Wcast-align"
bool SimpleSockUDP::GetInterfaceInfo(const string& interfaceName, unsigned int socket, struct in_addr *interfaceAddr, struct in_addr *netmaskAddr)
{
	struct ifreq interface;

    if(interfaceName=="")
    {
        bool ret;
        ret = GetInterfaceInfo("eth0", socket, interfaceAddr, netmaskAddr);
        if(ret) return true;
        return GetInterfaceInfo("egiga0", socket, interfaceAddr, netmaskAddr);
    }

	memset((char*)&interface, 0, sizeof(interface));
	interface.ifr_addr.sa_family = AF_INET;
	interface.ifr_broadaddr.sa_family = AF_INET;
    strcpy_s(interface.ifr_name, IFNAMSIZ, interfaceName.c_str());

	if ((ioctl(socket, SIOCGIFADDR, &interface))!=0) return false;
	memcpy_s(interfaceAddr, sizeof(struct in_addr), &((struct sockaddr_in*)&interface.ifr_broadaddr)->sin_addr, sizeof(struct in_addr));

	if ((ioctl(socket, SIOCGIFNETMASK, &interface))!=0) return false;
	memcpy_s(netmaskAddr,   sizeof(struct in_addr), &((struct sockaddr_in*)&interface.ifr_broadaddr)->sin_addr, sizeof(struct in_addr));

	return true;
}
#pragma GCC diagnostic pop
#else
#ifdef __mingw32__
#pragma GCC diagnostic push         //To conserve the same prototype between WIN32 and LINUX
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
bool SimpleSockUDP::GetInterfaceInfo(const string& interfaceAddress, unsigned int socket, struct in_addr *interfaceAddr, struct in_addr *netmaskAddr)
{
	DWORD bytesReturned;
	u_long flags;
	INTERFACE_INFO localAddr[10];  //Search the network interface in the top 10 IP interfaces
	int numLocalAddr;
	int i;
    unsigned long searchAddress;


    searchAddress = 0;
    if(interfaceAddress!="")
    {
        //searchAddress = inet_addr(interfaceAddress.c_str());
		inet_pton(AF_INET, interfaceAddress.c_str(), &searchAddress);
        if(searchAddress == INADDR_NONE) searchAddress = 0;
        if(searchAddress == INADDR_ANY) searchAddress = 0;
    }

	if ((WSAIoctl(socket, SIO_GET_INTERFACE_LIST, nullptr, 0, &localAddr, sizeof(localAddr), &bytesReturned, nullptr, nullptr)) == SOCKET_ERROR) return false;

	numLocalAddr = (bytesReturned/sizeof(INTERFACE_INFO));
	for (i=0; i<numLocalAddr; i++)
	{
        if(searchAddress != 0)
        {
            if(localAddr[i].iiAddress.AddressIn.sin_addr.s_addr != searchAddress) continue;
        }
        else
        {
            flags = localAddr[i].iiFlags;
            if(!(flags & IFF_UP)) continue;             //Pas démarré
            if(!(flags & IFF_BROADCAST)) continue;      //Pas de broadcast
            if(!(flags & IFF_MULTICAST)) continue;      //Pas de multicast
            if (flags & IFF_LOOPBACK) continue;         //C'est le loopback
            if (flags & IFF_POINTTOPOINT) continue;     //C'est le point à point
        }

        memcpy_s(interfaceAddr, sizeof(struct in_addr), &localAddr[i].iiAddress.AddressIn.sin_addr, sizeof(struct in_addr));
        memcpy_s(netmaskAddr,   sizeof(struct in_addr), &localAddr[i].iiNetmask.AddressIn.sin_addr, sizeof(struct in_addr));

		break;
	}

	return true;
}
#ifdef __mingw32__
#pragma GCC diagnostic pop
#endif
#endif

void SimpleSockUDP::SetSocket(SOCKET sockHandle, struct sockaddr_in sockAddress, int type)
{ SimpleSock::SetSocket(sockHandle, sockAddress, type); }

void SimpleSockUDP::Open(int port, const std::string& ipAddress)
{ SimpleSock::Open(port, ipAddress); }

void SimpleSockUDP::Open(int port, unsigned long ipAddress)
{ SimpleSock::Open(port, ipAddress); }

bool SimpleSockUDP::isOpen()
{ return SimpleSock::isOpen(); }

void SimpleSockUDP::Close()
{ SimpleSock::Close(); }

void SimpleSockUDP::Listen(int port, const std::string& ipAddress)
{ SimpleSock::Listen(port, ipAddress); }

void SimpleSockUDP::Listen(int port, unsigned long ipAddress)
{ SimpleSock::Listen(port, ipAddress); }

bool SimpleSockUDP::WaitRecv(int delay)
{ return SimpleSock::WaitRecv(delay); }

void SimpleSockUDP::Send(const std::string& buffer)
{ SimpleSock::Send(buffer); }

void SimpleSockUDP::Send(const char* buffer, size_t bufferSize)
{ SimpleSock::Send(buffer, bufferSize); }

bool SimpleSockUDP::Recv(std::string& buffer, int sizeMax)
{ return SimpleSock::Recv(buffer, sizeMax); }

unsigned SimpleSockUDP::Recv(char* buffer, size_t bufferSize)
{ return SimpleSock::Recv(buffer, bufferSize); }

void SimpleSockUDP::Blocking(bool blocking)
{ SimpleSock::Blocking(blocking); }

int SimpleSockUDP::GetPort()
{ return SimpleSock::GetPort(); }

std::string SimpleSockUDP::GetAddress()
{ return SimpleSock::GetAddress(); }
