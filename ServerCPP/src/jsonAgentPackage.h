
#ifndef _ADCS__jsonAgentPackage_H_
#define _ADCS__jsonAgentPackage_H_

#include "PackageInterface.h"
#include "json.hpp"

class jsonAgentPacket : public IPacket
{
public:
    jsonAgentPacket();
    jsonAgentPacket(const char* pData, unsigned int nDataLen, int clientSocket);
    
    virtual ~jsonAgentPacket();
    virtual void Process();
    virtual bool IsValid() const;
    
public:
    void BuildRegisterRequest(std::string strIP, int tcpport, int rpcport);
    void BuildUnRegisterRequest();
    void BuildGetServerListRequest(std::string serverType);
    nlohmann::json GetServerList();
    std::string GetValue();
    
protected:
    virtual bool GetResult(char*& pStreamData, unsigned long& ulDataLen);
    
private:
    nlohmann::json m_json_request;
    nlohmann::json m_json_result;
    
    nlohmann::json __process_one_operation__(nlohmann::json jrequest);
};

#endif
