
#ifndef _ADCS__SrvApp_H_
#define _ADCS__SrvApp_H_


#include "ThreadPool.h"
#include "GLog.h"
#include "ServerInterface.h"
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
    
    std::string         m_runmode;
    std::string         m_serverAddr;
    
public:
    bool Start(unsigned short usPort, std::string sMode, std::string sAddr);
    bool Stop();
    
    
    CServerApp();
    ~CServerApp();
    
    
};


#endif
