
#ifndef _ADCS__jsonPaxosPackage_H_
#define _ADCS__jsonPaxosPackage_H_

#include "PackageInterface.h"
#include "json.hpp"

enum class PaxosType : unsigned int
{
    Prepare = 1,
    PrepareResponse,
    Accept,
    AcceptResponse,
    
    ChosenValue,
};

class jsonPaxos : public IPacket
{
public:
    jsonPaxos(const char* pData, unsigned int nDataLen, int clientSocket);
    
    virtual ~jsonPaxos();
    virtual void Process();
    virtual bool IsValid() const;
    virtual bool NeedResponse() const;
    
    PaxosType Type() const;
    
    
protected:
    virtual bool GetResult(char*& pStreamData, unsigned long& ulDataLen);
    
private:
    nlohmann::json m_json_request;
    nlohmann::json m_json_result;

    
    PaxosType m_type;
};

#endif
