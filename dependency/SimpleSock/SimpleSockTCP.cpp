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
#include <exception>
#include "SimpleSockTCP.h"
using namespace std;

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSockTCP                                                                         ***/
/***                                                                                             ***/
/***************************************************************************************************/
SimpleSockTCP::SimpleSockTCP() : SimpleSock(SOCK_STREAM)
{
}

void SimpleSockTCP::Connect(const string& ipAddress, int port)
{
	unsigned long addr; //= inet_addr(ipAddress.c_str());
	
	inet_pton(AF_INET, ipAddress.c_str(), &addr);
	SimpleSock::Open(port, addr);

    if(connect(m_sockHandle,(sockaddr *) &m_sockAddress, sizeof(sockaddr)) == -1)
        throw SimpleSockTCP::Exception(0x0061, "SimpleSockTCP::Connect: unable to connect", GetSocketError());
}

bool SimpleSockTCP::Accept(ISimpleSockTCP* sockAccept)
{
	ARG_ACCEPT arg;
	SOCKET	sockClient;
	sockaddr_in	sockAddr;


    arg = sizeof sockAddr;
    sockClient = accept(m_sockHandle, (sockaddr *)&sockAddr, &arg);
    if(sockClient==0) return false;

    if(sockAccept->isOpen()) sockAccept->Close();
    sockAccept->SetSocket(sockClient, sockAddr, SOCK_STREAM);

    return true;
}

void SimpleSockTCP::Listen(int port)
{
    Listen(port, INADDR_ANY);
}

void SimpleSockTCP::SetSocket(SOCKET sockHandle, struct sockaddr_in sockAddress, int type)
{ SimpleSock::SetSocket(sockHandle, sockAddress, type); }

void SimpleSockTCP::Open(int port, const std::string& ipAddress)
{ SimpleSock::Open(port, ipAddress); }

void SimpleSockTCP::Open(int port, unsigned long ipAddress)
{ SimpleSock::Open(port, ipAddress); }

bool SimpleSockTCP::isOpen()
{ return SimpleSock::isOpen(); }

void SimpleSockTCP::Close()
{ SimpleSock::Close(); }

void SimpleSockTCP::Listen(int port, const std::string& ipAddress)
{ SimpleSock::Listen(port, ipAddress); }

void SimpleSockTCP::Listen(int port, unsigned long ipAddress)
{ SimpleSock::Listen(port, ipAddress); }

bool SimpleSockTCP::WaitRecv(int delay)
{ return SimpleSock::WaitRecv(delay); }

void SimpleSockTCP::Send(const std::string& buffer)
{ SimpleSock::Send(buffer); }

void SimpleSockTCP::Send(const char* buffer, size_t bufferSize)
{ SimpleSock::Send(buffer, bufferSize); }

bool SimpleSockTCP::Recv(std::string& buffer, int sizeMax)
{ return SimpleSock::Recv(buffer, sizeMax); }

unsigned SimpleSockTCP::Recv(char* buffer, size_t bufferSize)
{ return SimpleSock::Recv(buffer, bufferSize); }

void SimpleSockTCP::Blocking(bool blocking)
{ SimpleSock::Blocking(blocking); }

int SimpleSockTCP::GetPort()
{ return SimpleSock::GetPort(); }

std::string SimpleSockTCP::GetAddress()
{ return SimpleSock::GetAddress(); }

