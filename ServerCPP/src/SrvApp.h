
#ifndef _ADCS__SrvApp_H_
#define _ADCS__SrvApp_H_


#include "ThreadPool.h"
#include "GLog.h"
#include "ServerInterface.h"

class TCPServerProcessor;

class CServerApp
{
private:
    ADCS::IServer*      m_tcpServer;
    ADCS::CThreadPool*  m_pTcpThreadPool;
    ILog*               m_pLogger;
    TCPServerProcessor* m_processor;
    
public:
    bool Start(unsigned short usPort);
    bool Stop();
    
    
    CServerApp();
    ~CServerApp();
    
    
};


#endif
