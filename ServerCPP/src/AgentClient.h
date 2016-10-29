#ifndef __ADC_AgentClient_H__
#define __ADC_AgentClient_H__

#include <pthread.h>
#include "IClient.h"
#include "TCPClient.h"
#include "GLog.h"

namespace ADCS
{
    class CAgentClient
    {
    private:
        static void* ClientMain( void* pParam );
        
    private:
        pthread_t   AgentClientMainThreadId;
        CTCPClient  m_tcpClient;
        ILog*       m_logger;
        
    public:
        bool Start(ILog * pLogger );
        bool Stop();
        IClient* GetClient();
        
        CAgentClient();
        ~CAgentClient();
    };
}

#endif
