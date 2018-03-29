/*** LICENCE ***************************************************************************************/
/*
  owfscpp - Simple cpp library 1-wire interface

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
    along with owfscpp.  If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************************/
#include <iostream>
#include <cstring>
#include "owfscpp.h"
using namespace std;

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class owfscpp                                                                               ***/
/***                                                                                             ***/
/***************************************************************************************************/
owfscpp::owfscpp(string server, unsigned port) : m_OwfsServer(server), m_OwfsPort(port), m_Timeout(5000), m_Version(0), m_ControlFlags(0x00000100), m_Persistence(false), m_PersistenceMode(PERSISTENCE_CLOSE), m_OwfsSock()
{
}

owfscpp::~owfscpp()
{
}

void owfscpp::SetOwserverFlag(OwserverFlag flag, bool active)
{
    switch(flag)
    {
        case owfscpp::Uncached :
            if(active)
                m_ControlFlags |= 0x00000020;
            else
                m_ControlFlags &= 0xFFFFFFDF;
            break;
        case owfscpp::Safemode :
            if(active)
                m_ControlFlags |= 0x00000010;
            else
                m_ControlFlags &= 0xFFFFFFEF;
            break;
        case owfscpp::Alias :
            if(active)
                m_ControlFlags |= 0x00000008;
            else
                m_ControlFlags &= 0xFFFFFFF7;
            break;
        case owfscpp::Persistence :
            SetPersistence(active);
            break;
        case owfscpp::Bus_ret :
            if(active)
                m_ControlFlags |= 0x00000002;
            else
                m_ControlFlags &= 0xFFFFFFFD;
            break;
        default :
            throw owfscpp::Exception(0x0101, "owfscpp::SetOwserverFlag : Unknown enum value");
    }
}

unsigned owfscpp::GetOwserverFlags()
{
    return m_ControlFlags;
}

void owfscpp::SetPersistence(bool persistence)
{
    if(m_Persistence == persistence) return;

    m_PersistenceMode = PERSISTENCE_CLOSE;
    m_Persistence = persistence;

    if(m_Persistence)
    {
        m_ControlFlags |= 0x00000004;
    }
    else
    {
        Close();
        m_ControlFlags &= 0xFFFFFFFB;
    }
}

void owfscpp::SetDeviceDisplay(FormatDevice format)
{
    switch(format)
    {
        case owfscpp::FaPtId :
            m_ControlFlags &= 0xF0FFFFFF;
            break;
        case owfscpp::FaId :
            m_ControlFlags &= 0xF0FFFFFF;
            m_ControlFlags |= 0x01000000;
            break;
        case owfscpp::FaPtIdPtCo :
            m_ControlFlags &= 0xF0FFFFFF;
            m_ControlFlags |= 0x02000000;
            break;
        case owfscpp::FaPtIdCo :
            m_ControlFlags &= 0xF0FFFFFF;
            m_ControlFlags |= 0x03000000;
            break;
        case owfscpp::FaIdPtCo :
            m_ControlFlags &= 0xF0FFFFFF;
            m_ControlFlags |= 0x04000000;
            break;
        case owfscpp::FaIdCo :
            m_ControlFlags &= 0xF0FFFFFF;
            m_ControlFlags |= 0x05000000;
            break;
        default :
            throw owfscpp::Exception(0x0201, "owfscpp::SetDeviceDisplay : Unknown enum value");
    }
}

void owfscpp::SetTemperatureScale(TemperatureScale scale)
{
    switch(scale)
    {
        case owfscpp::Centigrade :
            m_ControlFlags &= 0xFFFCFFFF;
            break;
        case owfscpp::Fahrenheit :
            m_ControlFlags &= 0xFFFCFFFF;
            m_ControlFlags |= 0x00010000;
            break;
        case owfscpp::Kelvin :
            m_ControlFlags &= 0xFFFCFFFF;
            m_ControlFlags |= 0x00020000;
            break;
        case owfscpp::Rankine :
            m_ControlFlags &= 0xFFFCFFFF;
            m_ControlFlags |= 0x00030000;
            break;
        default :
            throw owfscpp::Exception(0x0301, "owfscpp::SetTemperatureScale : Unknown enum value");
    }
}

void owfscpp::SetPressureScale(PressureScale scale)
{
    switch(scale)
    {
        case owfscpp::Mbar :
            m_ControlFlags &= 0xFFE3FFFF;
            break;
        case owfscpp::Atm :
            m_ControlFlags &= 0xFFE3FFFF;
            m_ControlFlags |= 0x00040000;
            break;
        case owfscpp::MmHg :
            m_ControlFlags &= 0xFFE3FFFF;
            m_ControlFlags |= 0x00080000;
            break;
        case owfscpp::InHg :
            m_ControlFlags &= 0xFFE3FFFF;
            m_ControlFlags |= 0x000C0000;
            break;
        case owfscpp::Psi :
            m_ControlFlags &= 0xFFE3FFFF;
            m_ControlFlags |= 0x00100000;
            break;
        case owfscpp::Pa :
            m_ControlFlags &= 0xFFE3FFFF;
            m_ControlFlags |= 0x00140000;
            break;
        default :
            throw owfscpp::Exception(0x0401, "owfscpp::SetPressureScale : Unknown enum value");
    }
}

void owfscpp::Initialisation(string server, unsigned port)
{
    m_OwfsServer = server;
    m_OwfsPort = port;
}

void owfscpp::Connect()
{
    if((m_Persistence==true)&&(m_PersistenceMode==PERSISTENCE_OPEN)) return;
    try
    {
        m_OwfsSock.Connect(m_OwfsServer, m_OwfsPort);
    }
    catch(const exception &e)
    {
        ostringstream errorMsg;

        errorMsg << "owfscpp::Connect : Unable to connect to the owserver "<<m_OwfsServer<<":"<<m_OwfsPort<<" inner exception -> ("<<e.what()<<")";
        throw owfscpp::Exception(0x0501, errorMsg.str());
    }
    if(m_Persistence==true) m_PersistenceMode = PERSISTENCE_OPEN;
}

void owfscpp::Close()
{
    if(m_Persistence==true) return;
    m_OwfsSock.Close();
}

void owfscpp::WaitRecv()
{
    if(m_OwfsSock.WaitRecv(m_Timeout)) return;
    m_OwfsSock.Close();
    throw owfscpp::Exception(0x0001, "owfscpp::WaitRecv : Timeout");
}

char* owfscpp::WriteInt32(char* buffer, unsigned value)
{
    unsigned* intBuf = reinterpret_cast<unsigned*>(buffer);
    *intBuf = htonl(value);
    buffer+=4;
    return buffer;
}

int owfscpp::ReadInt32(char* buffer, unsigned pos)
{
    int* intBuf = reinterpret_cast<int*>(buffer+(pos-1)*4);
    return ntohl(*intBuf);
}

void owfscpp::SendMessage(unsigned message, const string& path, const string& value="")
{
    char* sendBuffer;
    char* pos;
    int length;
    int total;


    length = path.size()+1;
    if(value!="") length += value.size()+1;
    total = length + 6*4;
    sendBuffer = new char[total+1];
    pos = sendBuffer;
    pos = WriteInt32(pos, m_Version);
    pos = WriteInt32(pos, length);
    pos = WriteInt32(pos, message);
    pos = WriteInt32(pos, m_ControlFlags);
    if(value!="")
        pos = WriteInt32(pos, value.size());
    else
        pos = WriteInt32(pos, 1024);
    pos = WriteInt32(pos, 0);
    length = total+1-6*4;
    strcpy_s(pos, length, path.c_str());
    if(value!="")
    {
        length = length-(path.size()+1);
        strcpy_s(pos+path.size()+1, length, value.c_str());
    }

    try
    {
        m_OwfsSock.Send(sendBuffer, total);
    }
    catch(SimpleSock::Exception &e)
    {
        if(!m_Persistence) throw;
        m_PersistenceMode = PERSISTENCE_RETRY;
        Connect();
        m_OwfsSock.Send(sendBuffer, total);
    }

    delete[] sendBuffer;
}

string owfscpp::RecvBlock()
{
    char header[24];
    string recv;
    int len;
    int offset=0;
    unsigned readCtrlFlags;


    do
    {
        int err;

        if(m_OwfsSock.Recv(header, sizeof(header))!=24)
        {
            Close();
            throw owfscpp::Exception(0x0002, "owfscpp::RecvBlock : Error while reading the header");
        }

        err = ReadInt32(header, 3);
        if(err<0)
        {
            //cout << "Version : " << ReadInt32(header, 1) << endl;
            //cout << "PayLoad : " << ReadInt32(header, 2) << endl;
            //cout << "Return : " << ReadInt32(header, 3) << endl;
            //cout << "Control : 0x" << hex << ReadInt32(header, 4) << dec << endl;
            //cout << "Size : " << ReadInt32(header, 5) << endl;
            //cout << "Offset : " << ReadInt32(header, 6) << endl;
            Close();
            throw owfscpp::Exception(0x0003, "owfscpp::RecvBlock : Error return by owserver", err);
        }

        if(m_Persistence)
        {
            readCtrlFlags = ReadInt32(header, 4);
            if((readCtrlFlags&0x0004)==0)
            {
                m_Persistence = false;
                m_ControlFlags &= 0xFFFB;
            }
        }

        len = ReadInt32(header, 2);

    } while(len<0);    //Gestion du ping

    if(len==0) return "";

    m_OwfsSock.Recv(recv, len);
    while(recv.at(offset)==' ') offset++;
    if(offset>0) recv.erase(0, offset);
    return recv;
}

list<string> owfscpp::Dir(const string& path)
{
    list<string> listDir;
    string device;

    Connect();
    SendMessage(OWFSMSG_DIR, path);
    WaitRecv();
    while((device = RecvBlock())!="") listDir.push_back(device);
    Close();

    return listDir;
}

list<string> owfscpp::DirAll(const string& path)
{
    list<string> listDir;
    string device;
    string lstDevices;

    Connect();
    SendMessage(OWFSMSG_DIRALL, path);
    WaitRecv();
    lstDevices = RecvBlock();
    Close();

    istringstream iss(lstDevices);
    while(getline(iss, device, ',')) listDir.push_back(device);

    return listDir;
}

list<string> owfscpp::DirAllSlash(const string& path)
{
    list<string> listDir;
    string device;
    string lstDevices;

    Connect();
    SendMessage(OWFSMSG_DIRALLSLASH, path);
    WaitRecv();
    lstDevices = RecvBlock();
    Close();

    istringstream iss(lstDevices);
    while(getline(iss, device, ',')) listDir.push_back(device);

    return listDir;
}

bool owfscpp::Present(const std::string& path)
{
    bool ok = true;

    Connect();
    SendMessage(OWFSMSG_PRESENT, path);
    WaitRecv();
    try
    {
        RecvBlock();
    }
    catch(owfscpp::Exception &e)
    {
        if(e.GetOwfsError()!=-1) throw;
        ok = false;
    }
    Close();

    return ok;
}

void owfscpp::Write(const string& path, const string& value)
{
    Connect();
    SendMessage(OWFSMSG_WRITE, path, value);
    WaitRecv();
    RecvBlock();
    Close();
}

string owfscpp::Read(const string& path)
{
    return ReadSkip85(path, true);
}

string owfscpp::Get(const string& path)
{
    return GetSkip85(path, true);
}

string owfscpp::ReadSkip85(const string& path, bool skipBug85)
{
    string value;

    Connect();
    SendMessage(OWFSMSG_READ, path);
    WaitRecv();
    value = RecvBlock();
    Close();

    if(skipBug85) value = SkipBug85(path, value, false);

    return value;
}

string owfscpp::GetSkip85(const string& path, bool skipBug85)
{
    string value;

    Connect();
    SendMessage(OWFSMSG_GET, path);
    WaitRecv();
    value = RecvBlock();
    Close();

    if(skipBug85) value = SkipBug85(path, value, true);

    return value;
}

string owfscpp::SkipBug85(const string& path, const string& value, bool isGet)
{
    size_t lastSlash;
    bool isReadUncached;
    string newValue;

    if(value!="85") return value;

    lastSlash = path.find_last_of("/", path.length()-2);
    if(lastSlash==string::npos) return value;

    if(path.substr(lastSlash+1, 11)!="temperature") return value;

    isReadUncached = (m_ControlFlags & 0xFFFFFFDF) > 0;
    if(!isReadUncached) SetOwserverFlag(owfscpp::Uncached, true);
    if(isGet)
        newValue = GetSkip85(path, false);
    else
        newValue = ReadSkip85(path, false);
    if(!isReadUncached) SetOwserverFlag(owfscpp::Uncached, false);
    if(newValue=="85") throw owfscpp::Exception(0x000F, "owfscpp::Read : Unable to read temperature, always at 85");;

    return newValue;
}

string owfscpp::GetSlash(const string& path)
{
    string value;

    Connect();
    SendMessage(OWFSMSG_GETSLASH, path);
    WaitRecv();
    value = RecvBlock();
    Close();

    return value;
}

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class owfscpp::Exception                                                                    ***/
/***                                                                                             ***/
/***************************************************************************************************/
owfscpp::Exception::Exception(int number, string const& message, int system) throw() : m_number(number), m_message(message), m_system(system), m_whatMsg("")
{
    SetWhatMsg();
}

void owfscpp::Exception::SetWhatMsg()
{
    ostringstream message;

    message << m_message;
    if(m_system!=0) message << " (owfs error " << m_system << ")";
    m_whatMsg = message.str();
}

owfscpp::Exception::~Exception() throw()
{
}

const char* owfscpp::Exception::what() const throw()
{
    return m_whatMsg.c_str();
}

int owfscpp::Exception::GetNumber() const throw()
{
    return m_number;
}

int owfscpp::Exception::GetOwfsError() const throw()
{
    return m_system;
}
