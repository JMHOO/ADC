#include "jsonPaxos.h"
#include <string.h>
#include <algorithm>
#include "ErrorCode.h"
#include "base64.h"

using namespace nlohmann;
using namespace ADCS;

jsonPaxos::jsonPaxos()
{
}

jsonPaxos::jsonPaxos(const jsonPaxos& rhs)
{
    m_header = rhs.m_header;
    m_clientsocket = rhs.m_clientsocket;
    
    m_data.length = rhs.m_data.length;
    m_data.data = new char[m_data.length];
    memcpy(m_data.data, rhs.m_data.data, m_data.length);
    
    m_json_request = rhs.m_json_request;
    m_json_result = rhs.m_json_result;
    m_paxosType = rhs.m_paxosType;
}

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
    
    if( m_json_request.is_object())
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

PaxosType jsonPaxos::GetMessageType()
{
    unsigned int type = 0;
    try
    {
        type = m_json_request["type"];
    }
    catch (std::domain_error e)
    {
        // wront parameter
    }
    
    m_paxosType = (PaxosType)type;
    return m_paxosType;
}

uint64_t jsonPaxos::GetInstanceID()
{
    uint64_t instanceid = 0;
    try
    {
        instanceid = m_json_request["instanceid"];
    }
    catch(std::domain_error e)
    {
        // wrong parameter, drop
    }
    return instanceid;
}

uint64_t jsonPaxos::GetProposalID()
{
    uint64_t proposalid = 0;
    try
    {
        proposalid = m_json_request["proposalid"];
    }
    catch(std::domain_error e)
    {
        // wrong parameter, drop
    }
    return proposalid;
}

uint64_t jsonPaxos::GetRejectPromiseID()
{
    uint64_t promiseid = 0;
    try {
        promiseid = m_json_request["promiseid"];
    } catch (std::domain_error e) {
        // wrong parameter
    }
    return promiseid;
}

uint64_t jsonPaxos::GetPreAcceptID()
{
    uint64_t preacceptID = 0;
    try {
        preacceptID = m_json_request["preacceptid"];
    } catch (std::domain_error e) {
        // wrong parameter
    }
    return preacceptID;
}

int jsonPaxos::GetPreAcceptNodeID()
{
    int nodeid = 0;
    try
    {
        nodeid = m_json_request["preacceptnodeid"];
    }
    catch(std::domain_error e)
    {
        // wrong parameter, drop
    }
    return nodeid;
}

int jsonPaxos::GetNodeID()
{
    int nodeid = 0;
    try
    {
        nodeid = m_json_request["nodeid"];
    }
    catch(std::domain_error e)
    {
        // wrong parameter, drop
    }
    return nodeid;
}

std::string jsonPaxos::GetValue()
{
    std::string v = "";
    try {
        v = m_json_request["value"];
    } catch (std::domain_error e) {
        // wrong parameter, drop
    }
    return Base64::decode(v);
}
void jsonPaxos::SetMessageType(PaxosType type)
{
    unsigned int uiType = (unsigned int)type;
    m_json_result["jsonpaxos"] = "1.0";
    m_json_result["type"] = uiType;
    m_paxosType = type;
}

void jsonPaxos::SetInstanceID(uint64_t instanceID)
{
    m_json_result["instanceid"] = instanceID;
}

void jsonPaxos::SetProposalID(uint64_t proposalID)
{
    m_json_result["proposalid"] = proposalID;
}

void jsonPaxos::SetRejectPromiseID(uint64_t promiseID)
{
    m_json_result["promiseid"] = promiseID;
}

void jsonPaxos::SetPreAcceptID(uint64_t preacceptID)
{
    m_json_result["preacceptid"] = preacceptID;
}

void jsonPaxos::SetPreAcceptNodeID(int preacceptNodeID)
{
    m_json_request["preacceptnodeid"] = preacceptNodeID;
}

void jsonPaxos::SetNodeID(int nodeID)
{
    m_json_result["nodeid"] = nodeID;
}

void jsonPaxos::SetValue(std::string v)
{
    m_json_result["value"] = Base64::encode(v);
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

std::string jsonPaxos::GetResult()
{
    return m_json_result.dump();
}
