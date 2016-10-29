#include <stdio.h>
#include "AgentClient.h"


namespace ADCS
{
    void* CAgentClient::ClientMain( void* pParam )
    {
        CAgentClient* pAgent = reinterpret_cast<CAgentClient*>(pParam);
        
        
        return 0;
    }
    
    CAgentClient::CAgentClient()
    {
        m_logger = NULL;
    }
    
    CAgentClient::~CAgentClient()
    {
        
    }

    IClient* CAgentClient::GetClient()
    {
        return &m_tcpClient;
    }
    
    bool CAgentClient::Start(ILog * pLogger )
    {
        bool bStarted = false;
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
        
        if( pthread_create( &AgentClientMainThreadId, &attr, ClientMain, (void *)this ) != 0 )
        {
            bStarted = false;
        }
        else
            bStarted = true;
        
        pthread_attr_destroy(&attr);
        
        return bStarted;
    }
    
    bool CAgentClient::Stop()
    {
        pthread_join( AgentClientMainThreadId, NULL );
        
        return true;
    }

}
