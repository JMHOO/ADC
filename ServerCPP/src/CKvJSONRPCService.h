#ifndef __CKvJSONRPCService_H_
#define __CKvJSONRPCService_H_

#include "CKvRPCServiceInterface.h"
#include "GLog.h"

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
    ILog* logger;
};

#endif
