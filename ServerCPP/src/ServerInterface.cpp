#include <stdio.h>
#include "ServerInterface.h"

namespace ADCS
{
    void* IServer::ServerMain( void* pParam )
    {
        ((IServer *)pParam)->Main();
        return 0;
    }
    
    bool IServer::Start(ADCS::CThreadPool* pThreadPool, ILog * pLogger )
    {
        bool bStarted = false;
        
        if(!Initialize( pThreadPool, pLogger ))
            return false;
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
        
        if( pthread_create( &ServerMainThreadId, &attr, ServerMain, (void *)this ) != 0 )
        {
            Close();
            bStarted = false;
        }
        else
            bStarted = true;
        
        pthread_attr_destroy(&attr);
        
        return bStarted;
    }
    
    bool IServer::Stop()
    {
        if( !Close() )
            return false;
        
        pthread_join( ServerMainThreadId, NULL );
        
        return true;
    }
}
