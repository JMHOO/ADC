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
    
private:
    ILog* logger;
};

#endif
