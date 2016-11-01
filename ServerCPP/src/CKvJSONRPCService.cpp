#include <stdio.h>
#include "CKvJSONRPCService.h"
#include "KVServer.h"
#include "ErrorCode.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace ADCS;

CKvJSONRPCService::CKvJSONRPCService(jsonrpc::AbstractServerConnector &connector, ILog* plogger) : ICKvRPCService(connector), logger(plogger)
{
    
}

Json::Value CKvJSONRPCService::Put(const std::string& param1, const std::string& param2)
{
    // generate a random client ID
    srand((unsigned)time(NULL));
    int nClientID = rand() % (30000);
    
    Json::Value result;
    
    CKVServer* kvserver = CKVServer::GetInstance();
    if( !kvserver)
    {
        result["code"] = 1024;
        result["message"] = "Internal Server Error";
        if(logger)logger->Error("JSON-RPC Service error: get KVServer instance failed.");
        return result;
    }
    
    ErrorCode::KVStore code = kvserver->SetValue(nClientID, param1, param2);
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
    // generate a random client ID
    srand((unsigned)time(NULL));
    int nClientID = rand() % (30000);
    
    Json::Value result;

    CKVServer* kvserver = CKVServer::GetInstance();
    if( !kvserver)
    {
        result["code"] = 1024;
        result["message"] = "Internal Server Error";
        if(logger)logger->Error("JSON-RPC Service error: get KVServer instance failed.");
        return result;
    }
    
    ErrorCode::KVStore code = kvserver->Delete(nClientID, param1);
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
    result["code"] = 0;
    result["message"] = "pre-put success";
    
    int nServerID = std::stoi(param1);
    int nClientID = std::stoi(param2);
    
    // cache the operation, waitting on coordinator's go message
    Wait_Operation(nServerID, nClientID, "put", param3, param4);
    
    return result;
}

Json::Value CKvJSONRPCService::SrvDelete(const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4)
{
    Json::Value result;
    result["code"] = 0;
    result["message"] = "pre-delete success";
    
    int nServerID = std::stoi(param1);
    int nClientID = std::stoi(param2);
    
    // cache the operation, waitting on coordinator's go message
    Wait_Operation(nServerID, nClientID, "delete", param3, param4);
    
    return result;
}

Json::Value CKvJSONRPCService::SrvGo(const std::string& param1, const std::string& param2)
{
    Json::Value result;
    result["code"] = 0;
    result["message"] = "go success";
    
    int nServerID = std::stoi(param1);
    int nClientID = std::stoi(param2);
    
    Go_Operation(nServerID, nClientID);
    
    return result;
}

void CKvJSONRPCService::Wait_Operation(int nServerID, int nClientID, std::string opName, std::string key, std::string value)
{
    KVOperation op;
    op.name = "put";
    op.key = key;
    op.value = value;
    
    if(logger)logger->Info("JSON-RPC Service: cache operation: server[%d], client[%d] --- %s,%s,%s", nServerID, nClientID, op.name.c_str(), op.key.c_str(), op.value.c_str());
    
    ServerOP::iterator itServer = m_operations.find(nServerID);
    if( itServer != m_operations.end())
    {
        itServer->second.insert(ClientOP::value_type(nClientID, op));
    }
    else
    {
        ClientOP clientOPMap;
        clientOPMap.insert(ClientOP::value_type(nClientID, op));
        
        m_operations.insert(ServerOP::value_type(nServerID, clientOPMap));
    }
}

void CKvJSONRPCService::Go_Operation(int nServerID, int nClientID)
{
    CKVServer* kvserver = CKVServer::GetInstance();
    
    ServerOP::iterator itServer = m_operations.find(nServerID);
    if( itServer!= m_operations.end())
    {
        ClientOP::iterator itClient = itServer->second.find(nClientID);
        if(itClient != itServer->second.end())
        {
            KVOperation& op = itClient->second;
            if(logger)logger->Info("JSON-RPC Service: release operation: server[%d], client[%d] --- %s,%s,%s", nServerID, nClientID, op.name.c_str(), op.key.c_str(), op.value.c_str() );

            if( op.name == "put" )
            {
                // this kv operation does not need coordinator
                kvserver->SetValue(op.key, op.value);
            }
            else if( op.name == "delete")
            {
                // this kv operation does not need coordinator
                kvserver->Delete(op.key);
            }
            
            itClient = itServer->second.erase(itClient);
        }
    }
}
