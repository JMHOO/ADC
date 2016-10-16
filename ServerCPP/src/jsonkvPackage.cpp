#include "jsonkvPackage.h"
#include "KVServer.h"
#include <string.h>
#include "ErrorCode.h"

using namespace nlohmann;
using namespace ADCS;

jsonkvPacket::jsonkvPacket(const char* pData, unsigned int nDataLen, int clientSocket) : IPacket(pData, nDataLen, clientSocket)
{
    try{
        m_json_request = json::parse(m_data.data);
    }catch(std::invalid_argument arg)
    {
        printf("parser json error: %s", arg.std::exception::what());
        // paser json error
    }
}

jsonkvPacket::~jsonkvPacket()
{
    
}

void jsonkvPacket::Process()
{
    if( m_json_request.is_array())
    {
        for(size_t i = 0; i < m_json_request.size(); i++)
        {
            m_json_result.push_back(__process_one_operation__(m_json_request[i]));
        }
    }
    else if(m_json_request.is_object())
    {
        m_json_result = __process_one_operation__(m_json_request);
    }
}

json jsonkvPacket::__process_one_operation__(json jrequest)
{
    json jresult;
    jresult["jsonkv"] = "1.0";
    jresult["result"]["value"] = "0";
    jresult["result"]["code"] = "0";
    jresult["result"]["message"] = "success";
    jresult["id"] = jrequest["id"];
    
    // check the version of package
    if( jrequest["jsonkv"] != "1.0" )
    {
        jresult["result"]["code"] = "800";
        jresult["result"]["message"] = "Unsupported protocol";
    }
    
    CKVServer* kvserver = CKVServer::GetInstance();
    if( !kvserver)
    {
        jresult["result"]["code"] = "1024";
        jresult["result"]["code"] = "Internal Server Error";
        return jresult;
    }
    
    std::string kvOperator = jrequest["operate"];
    if( kvOperator == "put")
    {
        ErrorCode::KVStore code = kvserver->SetValue(jrequest["key"], jrequest["value"]);
        if( code == ErrorCode::KVStore::Success )
        {
            jresult["result"]["message"] = "put success";
        }
    }
    else if(kvOperator == "get")
    {
        std::string strValue = "";
        ErrorCode::KVStore code = kvserver->GetValue(jrequest["key"], strValue);
        if( code == ErrorCode::KVStore::Success )
        {
            jresult["result"]["value"] = strValue;
            jresult["result"]["message"] = "get success";
        }
        else
        {
            jresult["result"]["code"] = std::to_string((int)code);
            jresult["result"]["message"] = ErrorCode::Explain((unsigned int)code);
        }
    }
    else if(kvOperator == "delete")
    {
        ErrorCode::KVStore code = kvserver->Delete(jrequest["key"]);
        if( code == ErrorCode::KVStore::Success )
        {
            jresult["result"]["message"] = "Delete Success";
        }
        else
        {
            jresult["result"]["code"] = std::to_string((int)code);
            jresult["result"]["message"] = ErrorCode::Explain((unsigned int)code);
        }
    }
    else
    {
        jresult["result"]["code"] = "900";
        jresult["result"]["message"] = "Invalid Operator!";
    }
    
    return jresult;
}

bool jsonkvPacket::IsValid() const
{
    if(!IPacket::IsValid()) return false;
    
    if(m_json_request.is_array())
    {
        for(size_t i = 0; i < m_json_request.size(); i++)
        {
            auto j = m_json_request[i];
            if(j["jsonkv"] == "1.0")
                return true;
        }
    }
    else if( m_json_request.is_object())
    {
        if(m_json_request["jsonkv"] == "1.0")
            return true;
    }
    
    return false;
}

bool jsonkvPacket::GetResult(char*& pStreamData, unsigned long& ulDataLen)
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
