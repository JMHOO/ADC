
#include <stdio.h>
#include <string.h>
#include "RPCServer.h"


//#include <jsonrpccpp/server/connectors/unixdomainsocketserver.h>

namespace ADCS
{
    
bool CRPCServer::Initialize(CThreadPool* pool, ILog *plogger)
{
    if( status != ServerStatus::Uninit )
        return false;
    
    logger = plogger;
    status = ServerStatus::Inited;
    bExitFlag = false;
    
    return true;
}

bool CRPCServer::Main()
{
    if( status != ServerStatus::Inited )
        return false;

    struct timespec	ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 10000;     // 10ms
    
    try
    {
        httpserv = new jsonrpc::HttpServer(port);
        rpcserv = new CKvJSONRPCService(*httpserv);
        if (rpcserv->StartListening())
        {
            while(!bExitFlag)
            {
                nanosleep( &ts, NULL );
            }
        }
        else
        {
            if(logger)logger->Error("RPCServer start failed.");
        }
    }
    catch (jsonrpc::JsonRpcException& e)
    {
        if(logger)logger->Error("RPCServer error: %s", e.what());
    }
    
    return true;
}
    
bool CRPCServer::Close()
{
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
    
    return true;
}
bool CRPCServer::SetIP( const char *ip )
{
    if( strlen(ip) >= 16 )
        return false;
    
    strcpy( listenIPv4, ip );
    return true;
}

CRPCServer::CRPCServer():logger(NULL), port(0), status(ServerStatus::Uninit), rpcserv(NULL), httpserv(NULL)
{
    memset( listenIPv4, 0, 16);
}

CRPCServer::~CRPCServer()
{
    
}

}
