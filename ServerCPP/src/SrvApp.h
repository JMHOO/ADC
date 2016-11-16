
#ifndef _ADCS__SrvApp_H_
#define _ADCS__SrvApp_H_


#include "ThreadPool.h"
#include "GLog.h"
#include "ServerInterface.h"
#include "AgentClient.h"

#include <string>

#define INTERAL_SERVER_COUNT    3

class CServerApp
{
private:
    ADCS::IServer*          m_servers[INTERAL_SERVER_COUNT];       // TCP, UDP, RPC Server
    ADCS::CDiscoveryClient* m_discoveryClient;
    
    ADCS::IServer*          m_discoveryServer;   // Discovery Server
  
    
    std::string         m_runmode;
    std::string         m_serverExternalAddr;
    std::string         m_discoverySrvAddr;
    
public:
    bool Start(unsigned short usPort, std::string sMode, std::string serverExternalAddr, std::string discoveryServerAddr);
    bool Stop();
    
    void ListAllServer();
    
    
    CServerApp();
    ~CServerApp();
    
    
};


#endif
