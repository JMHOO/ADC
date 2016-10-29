
#ifndef _ADCS__jsonAgentPackage_H_
#define _ADCS__jsonAgentPackage_H_

#include "PackageInterface.h"
#include "json.hpp"

class jsonAgentPacket : public IPacket
{
public:
    jsonAgentPacket(const char* pData, unsigned int nDataLen, int clientSocket);
    
    virtual ~jsonAgentPacket();
    virtual void Process();
    virtual bool IsValid() const;
    
protected:
    virtual bool GetResult(char*& pStreamData, unsigned long& ulDataLen);
    
private:
    nlohmann::json m_json_request;
    nlohmann::json m_json_result;
    
    nlohmann::json __process_one_operation__(nlohmann::json jrequest);
};

#endif