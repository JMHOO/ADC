
#ifndef _ADCS__jsonkvPackage_H_
#define _ADCS__jsonkvPackage_H_

#include "PackageInterface.h"
#include "json.hpp"
#include<iostream>
#include<string>
using namespace std;
class jsonkvPacket : public IPacket
{
public:
    jsonkvPacket(const char* pData, unsigned int nDataLen, int clientSocket);
	jsonkvPacket(std::string pData);
    virtual ~jsonkvPacket();
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