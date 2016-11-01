#ifndef __CKvJSONRPCService_H_
#define __CKvJSONRPCService_H_

#include <map>
#include <string>
#include "CKvRPCServiceInterface.h"
#include "GLog.h"

using namespace std;

typedef struct __operation__
{
    string name;
    string key;
    string value;
}KVOperation;


// key1: server id, key2: client id
typedef map<int, KVOperation> ClientOP;
typedef map<int,ClientOP> ServerOP;

class CKvJSONRPCService : public ICKvRPCService
{
public:
    CKvJSONRPCService(jsonrpc::AbstractServerConnector &connector, ILog* plogger);
    
    virtual Json::Value Put(const std::string& param1, const std::string& param2);
    virtual Json::Value Get(const std::string& param1);
    virtual Json::Value Delete(const std::string& param1);
    virtual Json::Value SrvPut(const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4);
    virtual Json::Value SrvDelete(const std::string& param1, const std::string& param2, const std::string& param3, const std::string& param4);
    virtual Json::Value SrvGo(const std::string& param1, const std::string& param2);
    
private:
    void Wait_Operation(int nServerID, int nClientID, std::string opName, std::string key, std::string value);
    void Go_Operation(int nServerID, int nClientID);
    
    ILog* logger;
    ServerOP m_operations;
    
};

#endif
