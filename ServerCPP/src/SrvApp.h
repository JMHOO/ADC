
#ifndef _ADCS__SrvApp_H_
#define _ADCS__SrvApp_H_


#include "ThreadPool.h"
#include "GLog.h"
#include "ServerInterface.h"
#include "AgentClient.h"
#include <string>

class TCPServerProcessor;
class UDPServerProcessor;

class TCPServerProcessor;
class UDPServerProcessor;
class TCPSyncServerProcessor;

class AgentServerProcessor;

class CServerApp
{
private:
    ADCS::IServer*      m_tcpServer;
    ADCS::CThreadPool*  m_pTcpThreadPool;
    //TCPServerProcessor* m_tcpProcessor;
    TCPSyncServerProcessor* m_tcpProcessor;
    ILog*               m_pTcpLogger;

    ADCS::IServer*      m_udpServer;
    ADCS::CThreadPool*  m_pUdpThreadPool;
    UDPServerProcessor* m_udpProcessor;
    ILog*               m_pUdpLogger;
    
    ADCS::IServer*      m_rpcServer;
    ILog*               m_rpcLogger;
    
    ADCS::IServer*      m_agentServer;
    ADCS::CThreadPool*  m_agentThreadPool;
    AgentServerProcessor* m_agentProcessor;
    ILog*               m_agentLogger;
    
    ADCS::CAgentClient* m_agentClient;
    
    std::string         m_runmode;
    std::string         m_serverAddr;
    std::string         m_agentAddr;
    
public:
    bool Start(unsigned short usPort, std::string sMode, std::string serverSelfAddr, std::string agentServerAddr);
    bool Stop();
    
    
    CServerApp();
    ~CServerApp();
    
    
};


#endif
