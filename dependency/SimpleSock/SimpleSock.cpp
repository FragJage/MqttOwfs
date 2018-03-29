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
#include "SimpleSock.h"
using namespace std;

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSock                                                                            ***/
/***                                                                                             ***/
/***************************************************************************************************/
bool SimpleSock::m_initSocket = false;

void SimpleSock::InitSocket()
{
    SimpleSock::m_initSocket = true;

    #ifdef WIN32
    WSADATA	WsaData;
    if (WSAStartup(MAKEWORD(2,0), &WsaData) != 0)
        throw SimpleSock::Exception(0x0011, "SimpleSock::InitSocket: WSAStartup error", GetSocketError());
    #endif // WIN32
}

SimpleSock::SimpleSock(int type) : m_sockAddress(), m_sockHandle(INVALID_SOCKET), m_isOpen(false), m_sockType(type), m_blocking(false)
{
    m_sockAddress.sin_family = AF_INET;
    m_sockAddress.sin_port=0;
    m_sockAddress.sin_addr.s_addr=0;

    if(!SimpleSock::m_initSocket) InitSocket();
}

SimpleSock::~SimpleSock()
{
    Close();
}

void SimpleSock::SetSocket(SOCKET sockHandle, struct sockaddr_in sockAddress, int type)
{
    m_sockType = type;
    m_sockHandle = sockHandle;
    m_sockAddress = sockAddress;
    m_isOpen = true;
}

int SimpleSock::GetPort()
{
    return ntohs(m_sockAddress.sin_port);
}

string SimpleSock::GetAddress()
{
	char str[INET_ADDRSTRLEN];

	return string(inet_ntop(AF_INET, &(m_sockAddress.sin_addr), str, INET_ADDRSTRLEN));		//inet_ntoa(m_sockAddress.sin_addr);
}

void SimpleSock::Open(int port, unsigned long ipAddress)
{
    if(m_sockHandle == INVALID_SOCKET)
    {
        m_sockHandle = (int)socket(AF_INET, m_sockType, 0);
        if(m_sockHandle==0) throw  SimpleSock::Exception(0x0001, "SimpleSock::Open: unable to create the socket", GetSocketError());
    }

    memset(&m_sockAddress, 0, sizeof(m_sockAddress));
	m_sockAddress.sin_family = AF_INET;
	m_sockAddress.sin_port=htons(port);
    m_sockAddress.sin_addr.s_addr=ipAddress;
    m_isOpen = true;
}

void SimpleSock::Open(int port, const string& ipAddress)
{
	unsigned long addr; //= inet_addr(ipAddress.c_str());

	inet_pton(AF_INET, ipAddress.c_str(), &addr);
	Open(port, addr);
}

bool SimpleSock::isOpen()
{
    return m_isOpen;
}

void SimpleSock::Close()
{
    if(m_sockHandle != INVALID_SOCKET)
    {
        #ifdef WIN32
            shutdown(m_sockHandle, SD_BOTH);
        #else
            shutdown(m_sockHandle, SHUT_RDWR);
        #endif
        closesocket(m_sockHandle);
    }
 	m_sockHandle = INVALID_SOCKET;
    m_isOpen = false;
}

void SimpleSock::Listen(int port, const string& ipAddress)
{
	unsigned long addr; //= inet_addr(ipAddress.c_str());

	inet_pton(AF_INET, ipAddress.c_str(), &addr);
	Listen(port, addr);
}

void SimpleSock::Listen(int port, unsigned long ipAddress)
{
    int arg = 1;

    Open(port, ipAddress);

    switch(m_sockType)
    {
        case SOCK_DGRAM :
            if(setsockopt(m_sockHandle, SOL_SOCKET, SO_BROADCAST, (char*)&arg, sizeof(int))!=0)
                throw SimpleSock::Exception(0x0021, "SimpleSock::Listen: setsockopt(SO_BROADCAST) error", GetSocketError());
            break;

        case SOCK_STREAM :
            if(setsockopt(m_sockHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&arg, sizeof(int))!=0)
                throw SimpleSock::Exception(0x0022, "SimpleSock::Listen: setsockopt(SO_REUSEADDR) error", GetSocketError());
            break;

        default :
            throw SimpleSock::Exception(0x0024, "SimpleSock::Listen: unknown socket type");
    }

   	if(bind(m_sockHandle, (struct sockaddr*) &m_sockAddress, sizeof(m_sockAddress))!=0)
        throw  SimpleSock::Exception(0x0025, "SimpleSock::Listen: bind error", GetSocketError());

    if(m_sockType==SOCK_STREAM)
    {
        if(listen(m_sockHandle, 50)!=0)
            throw  SimpleSock::Exception(0x0027, "SimpleSock::Listen: listen error", GetSocketError());
    }
}

bool SimpleSock::WaitRecv(int delay)
{
	fd_set fsSock;
	struct timeval tvDelay;

    FD_ZERO(&fsSock);
    FD_SET(m_sockHandle, &fsSock);

    tvDelay.tv_sec=0;
    tvDelay.tv_usec=delay;

    select(m_sockHandle+1, &fsSock, nullptr, nullptr, &tvDelay);

    return FD_ISSET(m_sockHandle, &fsSock)>0;
}

void SimpleSock::Send(const char* buffer, size_t bufferSize)
{
    size_t sizeSent;
    size_t sizeMax;
    int status;


    if(bufferSize==0) throw SimpleSock::Exception(0x0044, "SimpleSock::Send: Invalid buffer size");
    if(buffer==nullptr) throw SimpleSock::Exception(0x0043, "SimpleSock::Send: Unable to send null buffer");
    if(!m_isOpen) throw SimpleSock::Exception(0x0042, "SimpleSock::Send: Open() method is mandatory before Send()");

    sizeMax = bufferSize;
    sizeSent = 0;

    while(sizeSent < sizeMax)
    {
        switch(m_sockType)
        {
            case SOCK_DGRAM :
                status = sendto(m_sockHandle, buffer + sizeSent, sizeMax - sizeSent, 0, (struct sockaddr *)&m_sockAddress, sizeof(struct sockaddr));
                break;

            case SOCK_STREAM :
                status = send(m_sockHandle, buffer + sizeSent, sizeMax - sizeSent, 0);
                break;

            default :
                throw SimpleSock::Exception(0x0040, "SimpleSock::Send: unknown socket type");
        }

        if((status==-1)&&(!OperationOk()))
			throw SimpleSock::Exception(0x0041, "SimpleSock::Send: send error", GetSocketError());

		if(status != -1) sizeSent += status;
    }
}

void SimpleSock::Send(const string& buffer)
{
    Send(buffer.c_str(), buffer.size());
}

unsigned SimpleSock::Recv(char* buffer, size_t bufferSize)
{
	int status;


    if(bufferSize==0) throw SimpleSock::Exception(0x0054, "SimpleSock::Recv: Invalid buffer size");
    if(buffer==nullptr) throw SimpleSock::Exception(0x0053, "SimpleSock::Recv: Invalid buffer pointer");
    if(!m_isOpen) throw SimpleSock::Exception(0x0052, "SimpleSock::Recv: Listen() or Connect() method is mandatory before Recv()");

    switch(m_sockType)
    {
        case SOCK_DGRAM :
            status = recvfrom(m_sockHandle, buffer, bufferSize, 0, nullptr, nullptr);
            break;

        case SOCK_STREAM :
            status = recv(m_sockHandle, buffer, bufferSize, 0);
            break;

        default :
            throw SimpleSock::Exception(0x0050, "SimpleSock::Recv: unknown socket type");
    }

    if((status==-1)&&(!OperationOk()))
        throw SimpleSock::Exception(0x0051, "SimpleSock::Recv: recv[from] error", GetSocketError());

	return status;
}

bool SimpleSock::Recv(string& buffer, int sizMax)
{
	int status;
	char buf[1536];
	bool bRet;


    if(sizMax<-1) throw SimpleSock::Exception(0x0054, "SimpleSock::Recv: Invalid buffer size");
    if(!m_isOpen) throw SimpleSock::Exception(0x0052, "SimpleSock::Recv: Listen() or Connect() method is mandatory before Recv()");

    buffer = "";
    bRet = false;
    do
    {
        int sizBuf;

        sizBuf = sizeof(buf)-1;
        if((sizMax!=-1)&&(sizBuf>sizMax)) sizBuf=sizMax;
        switch(m_sockType)
        {
            case SOCK_DGRAM :
                status = recvfrom(m_sockHandle, buf, sizBuf, 0, nullptr, nullptr);
                break;

            case SOCK_STREAM :
                status = recv(m_sockHandle, buf, sizBuf, 0);
                break;

            default :
                throw SimpleSock::Exception(0x0050, "SimpleSock::Recv: unknown socket type");
        }

        if((status==-1)&&(!OperationOk()))
            throw SimpleSock::Exception(0x0051, "SimpleSock::Recv: recv[from] error", GetSocketError());

        if(status!=-1)
        {
            buf[status]='\0';
            buffer += buf;
            bRet = true;
            if(sizMax!=-1)
            {
                sizMax -= status;
                if(sizMax==0) status = -1;
            }
        }

    } while((status!=-1)&&(m_blocking==false));

	return bRet;
}

void SimpleSock::Blocking(bool blocking)
{
    int result;

    if(m_sockHandle == INVALID_SOCKET)
    {
        m_sockHandle = (int)socket(AF_INET, m_sockType, 0);
        if(m_sockHandle==0) throw  SimpleSock::Exception(0x0001, "SimpleSock::Blocking: unable to create the socket", GetSocketError());
    }

	#ifdef __linux__
        int flags;
        flags = fcntl(m_sockHandle, F_GETFL);
        if(blocking)
            flags = (flags & ~O_NONBLOCK);
        else
            flags = (flags | O_NONBLOCK);
        result = fcntl(m_sockHandle, F_SETFL, flags);
    #else
        u_long iMode = !blocking;
        result = ioctlsocket(m_sockHandle, FIONBIO, &iMode);
        if(result!=0) result = -1;
	#endif

    if (result == -1)
        throw SimpleSock::Exception(0x0031, "SimpleSock::Blocking: ioctl error", GetSocketError());

    m_blocking = blocking;
}

int SimpleSock::GetSocketError()
{
    #ifdef __linux__
        return errno;
    #else
        return WSAGetLastError();
    #endif
}

bool SimpleSock::OperationOk()
{
    int err;

    err = GetSocketError();
    #ifdef __linux__
        if(err == EAGAIN || err == EWOULDBLOCK) return true;
    #else
        if(err == WSAEWOULDBLOCK) return true;
    #endif
    return false;
}

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSock::Exception                                                                 ***/
/***                                                                                             ***/
/***************************************************************************************************/
SimpleSock::Exception::Exception(int number, string const& message) throw() : m_number(number), m_message(message), m_system(0), m_whatMsg()
{
    SetWhatMsg();
}

SimpleSock::Exception::Exception(int number, string const& message, int system) throw() : m_number(number), m_message(message), m_system(system), m_whatMsg()
{
    SetWhatMsg();
}

SimpleSock::Exception::~Exception() throw()
{
}

void SimpleSock::Exception::SetWhatMsg()
{
    ostringstream message;

    message << m_message;
    if(m_system!=0) message << " (system error " << m_system << ")";
    m_whatMsg = message.str();
}

const char* SimpleSock::Exception::what() const throw()
{
    return m_whatMsg.c_str();
}

int SimpleSock::Exception::GetNumber() const throw()
{
    return m_number;
}
