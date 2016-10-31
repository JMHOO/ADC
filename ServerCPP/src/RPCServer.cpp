
#include <stdio.h>
#include <string.h>
#include "RPCServer.h"


//#include <jsonrpccpp/server/connectors/unixdomainsocketserver.h>

namespace ADCS
{
    
    bool CRPCServer::Initialize()
    {
        if( status != ServerStatus::Uninit )
            return false;
        
        status = ServerStatus::Inited;
        bExitFlag = false;
        
        return true;
    }

    bool CRPCServer::Main()
    {
        if( status != ServerStatus::Inited )
            return false;

        //struct timespec	ts;
        //ts.tv_sec = 0;
        //ts.tv_nsec = 500000;     // 100ms
        
        try
        {
            httpserv = new jsonrpc::HttpServer(port);
            rpcserv = new CKvJSONRPCService(*httpserv, m_logger);
            if (rpcserv->StartListening())
            {
                if(m_logger)m_logger->Info("RPC Server Main thread waiting....");
                // waiting for exit signal.
                pthread_cond_wait(&Condition, &Mutex);
             
                //while(!bExitFlag)
                //{
                //    nanosleep( &ts, NULL );
                //}
                if(m_logger)m_logger->Info("RPC Server Main thread exiting....");
            }
            else
            {
                if(m_logger)m_logger->Error("RPCServer start failed.");
            }
        }
        catch (jsonrpc::JsonRpcException& e)
        {
            if(m_logger)m_logger->Error("RPCServer error: %s", e.what());
        }
        
        return true;
    }
        
    bool CRPCServer::Close()
    {
        if(m_logger)m_logger->Warning("RPC Server Closing, signal main thread");
        // release main thread
        pthread_cond_signal(&Condition);
        
        bExitFlag = true;
        if( rpcserv)
        {
            rpcserv->StopListening();
            delete rpcserv;
        }
        if( httpserv )
        {
            delete httpserv;
        }
        if(m_logger)m_logger->Warning("RPC server shutted down.");
        
        return true;
    }
    bool CRPCServer::SetIP( const char *ip )
    {
        if( strlen(ip) >= 16 )
            return false;
        
        strcpy( listenIPv4, ip );
        return true;
    }

    CRPCServer::CRPCServer(IExecuteor* executor): ADCS::IServer("RPC", executor), port(0), status(ServerStatus::Uninit), rpcserv(NULL), httpserv(NULL)
    {
        memset( listenIPv4, 0, 16);
        pthread_mutex_init(&Mutex, NULL );
        pthread_cond_init(&Condition, NULL );
    }

    CRPCServer::~CRPCServer()
    {
        pthread_mutex_destroy(&Mutex);
        pthread_cond_destroy(&Condition);
    }

}
