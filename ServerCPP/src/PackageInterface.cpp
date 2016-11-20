#include <string.h>
#include <arpa/inet.h>
#include "PackageInterface.h"
#include "jsonkvPackage.h"
#include "jsonAgentPackage.h"
#include "jsonPaxos.h"

IPacket* IPacket::CreatePackage(const char* sdata, unsigned int uiDataLen, int clientSocket)
{
    IPacket* packet = nullptr;
    if( sdata && uiDataLen >= ADCS::LENGTH_PACKHEADER )
    {
        ADCS::PACK_HEADER header;
        memset(&header, 0, sizeof(ADCS::PACK_HEADER));
        memcpy(&header, sdata, ADCS::LENGTH_PACKHEADER);
        
        if( ntohl(header.Length) <= uiDataLen )
        {
            // check package type
            // we can use Type field of PACK_HEADER
            // or just check the data header
            if( strstr(sdata+ADCS::LENGTH_PACKHEADER, "jsonkv") != NULL)
            {
                // jsonkv package
                packet = new jsonkvPacket(sdata, uiDataLen, clientSocket);
                
            }
            else if( strstr(sdata+ADCS::LENGTH_PACKHEADER, "jsonagent") != NULL)
            {
                // jsonagent package
                packet = new jsonAgentPacket(sdata, uiDataLen, clientSocket);
            }
            else if( strstr(sdata+ADCS::LENGTH_PACKHEADER, "jsonpaxos") != NULL)
            {
                packet = new jsonPaxos(sdata, uiDataLen, clientSocket);
            }
            
            if( packet && !packet->IsValid() )
            {
                delete packet;
                packet = nullptr;
            }
        }
    }
    return packet;
}

IPacket::IPacket() : m_type(ADCS::PackageType::None)
{
    m_clientsocket = 0;
    memset(&m_header, 0, ADCS::LENGTH_PACKHEADER);
}

IPacket::IPacket(const char* pData, unsigned int nDataLen, int clientSocket)
{
    m_clientsocket = clientSocket;
    if( pData && nDataLen >= ADCS::LENGTH_PACKHEADER )
    {
        memcpy(&m_header, pData, ADCS::LENGTH_PACKHEADER);
        m_header.Length = ntohl(m_header.Length);
        
        if( m_header.Length <= nDataLen )
        {// it seems we have data, create package
            m_data.length = m_header.Length - ADCS::LENGTH_PACKHEADER;
            
            m_data.data = new char[m_data.length+1];
            memset(m_data.data, 0, m_data.length+1);
            memcpy(m_data.data, pData+ADCS::LENGTH_PACKHEADER, m_data.length);
        }
        else
            memset(&m_header, 0, sizeof(ADCS::PACK_HEADER));
    }
}

IPacket::IPacket(const IPacket& rhs)
{
    m_header = rhs.m_header;
    m_clientsocket = rhs.m_clientsocket;
    
    m_data.length = rhs.m_data.length;
    m_data.data = new char[m_data.length];
    memcpy(m_data.data, rhs.m_data.data, m_data.length);
}

IPacket& IPacket::operator = (const IPacket& rhs)
{
    if( this != &rhs )
    {
        clear();
        
        m_header = rhs.m_header;
        m_clientsocket = rhs.m_clientsocket;
        
        m_data.length = rhs.m_data.length;
        m_data.data = new char[m_data.length];
        memcpy(m_data.data, rhs.m_data.data, m_data.length);
        
        m_bytesStream.length = rhs.m_bytesStream.length;
        m_bytesStream.data = new char[m_bytesStream.length];
        memcpy(m_bytesStream.data, rhs.m_bytesStream.data, m_bytesStream.length);
    }
    return *this;
}

IPacket::~IPacket()
{
    clear();
}

bool IPacket::IsValid() const
{
    if( m_header.Length == 0 || m_data.length == 0 || m_data.data == nullptr )
        return false;
    
    return true;
}

bool IPacket::NeedResponse() const
{
    return true;
}

ADCS::PackageType IPacket::Type() const
{
    return m_type;
}

bool IPacket::ToBytes(const char*& pStreamData, unsigned long& ulDataLen)
{
    clear_bytesStream();
    
    char* ptrResult = nullptr;
    unsigned long ulResultLen = 0;
    
    if( !GetResult(ptrResult, ulResultLen) )
        return false;
    
    
    m_bytesStream.length = (unsigned int)(ADCS::LENGTH_PACKHEADER + ulResultLen);
    m_bytesStream.data = new char[m_bytesStream.length+1];
    if( m_bytesStream.data == nullptr )
    {
        m_bytesStream.length = 0;
        return false;
    }
    memset(m_bytesStream.data, 0, m_bytesStream.length+1);
    
    ADCS::PACK_HEADER header;
    memset(&header, 0, sizeof(ADCS::PACK_HEADER));
    header.Length = htonl(m_bytesStream.length);
    
    memcpy(m_bytesStream.data, &header, sizeof(ADCS::PACK_HEADER));
    memcpy(m_bytesStream.data + ADCS::LENGTH_PACKHEADER, ptrResult, ulResultLen);
       
    pStreamData = m_bytesStream.data;
    ulDataLen = m_bytesStream.length;
    
    return true;
}

void IPacket::clear()
{
    if( m_data.data != nullptr )
    {
        delete[] m_data.data;
        m_data.data = nullptr;
        m_data.length = 0;
    }
    
    clear_bytesStream();
}

void IPacket::clear_bytesStream()
{
    if( m_bytesStream.data != nullptr )
    {
        delete[] m_bytesStream.data;
        m_bytesStream.data = nullptr;
        m_bytesStream.length = 0;
    }
}
