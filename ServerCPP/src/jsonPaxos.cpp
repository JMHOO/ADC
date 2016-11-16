#include "jsonPaxos.h"
#include <string.h>
#include <algorithm>
#include "ErrorCode.h"

using namespace nlohmann;
using namespace ADCS;

jsonPaxos::jsonPaxos(const char* pData, unsigned int nDataLen, int clientSocket) : IPacket(pData, nDataLen, clientSocket)
{
    m_type = PackageType::Paxos;
    
    try
    {
        m_json_request = json::parse(m_data.data);
    }
    catch(std::invalid_argument arg)
    {
        printf("parser json error: %s", arg.std::exception::what());
        // paser json error
    }
    catch(...)
    { }
}

jsonPaxos::~jsonPaxos()
{
}

void jsonPaxos::Process()
{
   
}
bool jsonPaxos::IsValid() const
{
    if(!IPacket::IsValid()) return false;
    
    if(m_json_request.is_array())
    {
        for(size_t i = 0; i < m_json_request.size(); i++)
        {
            auto j = m_json_request[i];
            if(j["jsonpaxos"] == "1.0")
                return true;
        }
    }
    else if( m_json_request.is_object())
    {
        if(m_json_request["jsonpaxos"] == "1.0")
            return true;
    }
    
    return false;
}

bool jsonPaxos::NeedResponse() const
{
    return true;
}

PaxosType jsonPaxos::MessageType() const
{
    return m_paxosType;
}


bool jsonPaxos::GetResult(char*& pStreamData, unsigned long& ulDataLen)
{
    std::string strResult = m_json_result.dump();
    
    ulDataLen = strResult.length();
    pStreamData = new char[ulDataLen];
    if(pStreamData == nullptr)
    {
        ulDataLen = 0;
        return false;
    }
    memcpy(pStreamData, strResult.c_str(), ulDataLen);
    
    return true;
}
