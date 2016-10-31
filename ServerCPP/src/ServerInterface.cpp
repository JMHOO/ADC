#include <stdio.h>
#include <algorithm>
#include "ServerInterface.h"
#include "TCPSyncProcessor.h"
#include "UDPSyncProcessor.h"
#include "AgentServProcessor.h"
#include "TCPSyncIOServer.h"
#include "UDPSyncIOServer.h"
#include "RPCServer.h"

namespace ADCS
{
    IServer* IServer::CreateServer(const char* sName)
    {
        IServer* pServer = nullptr;
        std::string strName = sName;
        std::transform(strName.begin(),strName.end(),strName.begin(), ::tolower);
        if( strName == "tcp" )
        {
            pServer = new CTCPSIOServer(strName.c_str(), new TCPSyncServerProcessor());
        }
        else if(strName == "udp")
        {
            pServer = new CUDPSyncIOServer(new UDPServerProcessor());
        }
        else if( strName=="discovery")
        {
            pServer = new CTCPSIOServer(strName.c_str(), new AgentServerProcessor());
        }
        else if( strName=="rpc")
        {
            pServer = new CRPCServer(nullptr);
        }
        
        return pServer;
    }
    
    void* IServer::ServerMain( void* pParam )
    {
        ((IServer *)pParam)->Main();
        return 0;
    }
    
    IServer::IServer(const char* sName, IExecuteor* executor)
    {
        m_serverName = sName;
        m_processor = executor;
        if( m_processor )
            m_pThreadPool = new CThreadPool();
        
        m_logger = new GlobalLog(sName, LL_DEBUG);
        if(m_processor)m_processor->SetLogger(m_logger);
    }
    
    IServer::~IServer()
    {
        if( m_processor ) delete m_processor;
        if( m_pThreadPool )
        {
            //m_pThreadPool->Destory();
            delete m_pThreadPool;
        }
        if( m_logger )delete m_logger;
    }
    
    bool IServer::Start(unsigned short usPort)
    {
        bool bStarted = false;
        
        if( m_processor && m_pThreadPool )m_pThreadPool->Init(*m_processor, 10, 20, 100);
        
        SetIP("0.0.0.0");
        SetPort(usPort);
        
        if(!Initialize())
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
        
        std::cout << m_serverName.c_str() << " server is started, "<< GetIP() << ":"<< GetPort()<<" LISTENING" << std::endl;
        if( m_logger)m_logger->Info("%s Server started: %s:%d LISTENING.", m_serverName.c_str() , GetIP(), GetPort());
        
        return bStarted;
    }
    
    bool IServer::Stop()
    {
        if( !Close() )
            return false;
        
        pthread_join( ServerMainThreadId, NULL );
        
        return true;
    }
    
    ILog* IServer::GetLogger()
    {
        return m_logger;
    }
}
