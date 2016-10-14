
#ifndef _ADCS__SrvApp_H_
#define _ADCS__SrvApp_H_


#include "ThreadPool.h"
#include "GLog.h"
#include "ServerInterface.h"

class TCPServerProcessor;
class UDPServerProcessor;

class CServerApp
{
private:
    ADCS::IServer*      m_tcpServer;
    ADCS::CThreadPool*  m_pTcpThreadPool;
    TCPServerProcessor* m_tcpProcessor;
    ILog*               m_pTcpLogger;

    ADCS::IServer*      m_udpServer;
    ADCS::CThreadPool*  m_pUdpThreadPool;
    UDPServerProcessor* m_udpProcessor;
    ILog*               m_pUdpLogger;
    
public:
    bool Start(unsigned short usPort);
    bool Stop();
    
    
    CServerApp();
    ~CServerApp();
    
    
};


#endif
