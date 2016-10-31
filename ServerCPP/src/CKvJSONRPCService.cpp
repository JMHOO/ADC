#include <stdio.h>
#include "CKvJSONRPCService.h"
#include "KVServer.h"
#include "ErrorCode.h"

using namespace ADCS;

CKvJSONRPCService::CKvJSONRPCService(jsonrpc::AbstractServerConnector &connector, ILog* plogger) : ICKvRPCService(connector), logger(plogger)
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
        if(logger)logger->Error("JSON-RPC Service error: get KVServer instance failed.");
        return result;
    }
    
    ErrorCode::KVStore code = kvserver->SetValue(param1, param2);
    if( code == ErrorCode::KVStore::Success )
    {
        result["code"] = 0;
        result["message"] = "put success";
        if(logger)logger->Info("JSON-RPC Service -- Put operation success, key:%s, value:%s", param1.c_str(), param2.c_str());
    }
    else
    {
        result["code"] = (unsigned int)code;
        result["message"] = ErrorCode::Explain((unsigned int)code);
        if(logger)logger->Warning("JSON-RPC Service -- Put operation failed, code:%d, reason:%s, key:%s, value:%s", (unsigned int)code, ErrorCode::Explain((unsigned int)code), param1.c_str(), param2.c_str());
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
        if(logger)logger->Error("JSON-RPC Service error: get KVServer instance failed.");
        return result;
    }
    
    std::string strValue = "";
    ErrorCode::KVStore code = kvserver->GetValue(param1, strValue);
    if( code == ErrorCode::KVStore::Success )
    {
        result["value"] = strValue;
        result["code"] = 0;
        result["message"] = "get success";
        if(logger)logger->Info("JSON-RPC Service -- Get operation success, key:%s, returned value:%s", param1.c_str(), strValue.c_str());
    }
    else
    {
        result["code"] = (unsigned int)code;
        result["message"] = ErrorCode::Explain((unsigned int)code);
        if(logger)logger->Warning("JSON-RPC Service -- Get operation failed, code:%d, reason:%s, key:%s", (unsigned int)code, ErrorCode::Explain((unsigned int)code), param1.c_str());
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
        if(logger)logger->Error("JSON-RPC Service error: get KVServer instance failed.");
        return result;
    }
    
    ErrorCode::KVStore code = kvserver->Delete(param1);
    if( code == ErrorCode::KVStore::Success )
    {
        result["code"] = 0;
        result["message"] = "Delete Success";
        if(logger)logger->Info("JSON-RPC Service -- Delete operation success, key:%s", param1.c_str());
    }
    else
    {
        result["code"] = (unsigned int)code;
        result["message"] = ErrorCode::Explain((unsigned int)code);
        if(logger)logger->Warning("JSON-RPC Service -- Delete operation failed, code:%d, reason:%s, key:%s", (unsigned int)code, ErrorCode::Explain((unsigned int)code), param1.c_str());
    }
    
    return result;
}

Json::Value CKvJSONRPCService::SrvPut(const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4)
{
    Json::Value result;
    
    return result;
}

Json::Value CKvJSONRPCService::SrvDelete(const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4)
{
    Json::Value result;
    
    return result;
}

Json::Value CKvJSONRPCService::SrvGo(const std::string& param1, const std::string& param2)
{
    Json::Value result;
    
    return result;
}
