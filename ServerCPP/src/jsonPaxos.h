
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
    jsonPaxos();
    jsonPaxos(const char* pData, unsigned int nDataLen, int clientSocket);
    jsonPaxos(const jsonPaxos& rhs);
    
    virtual ~jsonPaxos();
    virtual void Process();
    virtual bool IsValid() const;
    virtual bool NeedResponse() const;
    
    PaxosType GetMessageType();
    uint64_t GetInstanceID();
    uint64_t GetProposalID();
    uint64_t GetRejectPromiseID();
    uint64_t GetPreAcceptID();
    int GetPreAcceptNodeID();
    int GetNodeID();
    std::string GetValue();
    
    void SetMessageType(PaxosType type);
    void SetInstanceID(uint64_t instanceID);
    void SetProposalID(uint64_t proposalID);
    void SetRejectPromiseID(uint64_t promiseID);
    void SetPreAcceptID(uint64_t preacceptID);
    void SetPreAcceptNodeID(int preacceptNodeID);
    void SetNodeID(int nodeID);
    void SetValue(std::string value);
    
    std::string GetResult();
    
protected:
    virtual bool GetResult(char*& pStreamData, unsigned long& ulDataLen);
    
private:
    nlohmann::json m_json_request;
    nlohmann::json m_json_result;

    
    PaxosType m_paxosType;
};

#endif
