#include <stdio.h>
#include "CKvJSONRPCService.h"
#include "KVServer.h"
#include "ErrorCode.h"

using namespace ADCS;

CKvJSONRPCService::CKvJSONRPCService(jsonrpc::AbstractServerConnector &connector) : ICKvRPCService(connector)
{
    
}

Json::Value CKvJSONRPCService::Put(const std::string& param1, const std::string& param2)
{
    Json::Value result;
    
    CKVServer* kvserver = CKVServer::GetInstance();
    if( !kvserver)
    {
        result["code"] = 1024;
        result["message"] = "Internal Server Error";
        return result;
    }
    
    ErrorCode::KVStore code = kvserver->SetValue(param1, param2);
    if( code == ErrorCode::KVStore::Success )
    {
        result["code"] = 0;
        result["message"] = "put success";
    }
    else
    {
        result["code"] = (unsigned int)code;
        result["message"] = ErrorCode::Explain((unsigned int)code);
    }
    
    return result;
}

Json::Value CKvJSONRPCService::Get(const std::string& param1)
{
    Json::Value result;
    CKVServer* kvserver = CKVServer::GetInstance();
    if( !kvserver)
    {
        result["code"] = 1024;
        result["message"] = "Internal Server Error";
        return result;
    }
    
    std::string strValue = "";
    ErrorCode::KVStore code = kvserver->GetValue(param1, strValue);
    if( code == ErrorCode::KVStore::Success )
    {
        result["value"] = strValue;
        result["code"] = 0;
        result["message"] = "get success";
    }
    else
    {
        result["code"] = (unsigned int)code;
        result["message"] = ErrorCode::Explain((unsigned int)code);
    }

    return result;
}

Json::Value CKvJSONRPCService::Delete(const std::string& param1)
{
    Json::Value result;

    CKVServer* kvserver = CKVServer::GetInstance();
    if( !kvserver)
    {
        result["code"] = 1024;
        result["message"] = "Internal Server Error";
        return result;
    }
    
    ErrorCode::KVStore code = kvserver->Delete(param1);
    if( code == ErrorCode::KVStore::Success )
    {
        result["code"] = 0;
        result["message"] = "Delete Success";
    }
    else
    {
        result["code"] = (unsigned int)code;
        result["message"] = ErrorCode::Explain((unsigned int)code);
    }
    
    return result;
}
