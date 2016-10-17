#ifndef _ADC__RPCServer_H_
#define _ADC__RPCServer_H_

#include "Network.h"
#include "GLog.h"
#include "ServerInterface.h"
#include "ThreadPool.h"
#include "CKvJSONRPCService.h"
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>

namespace ADCS
{
class CRPCServer : public IServer
{
    ILog            *logger;
    unsigned short	port;
    char            listenIPv4[16];
    ServerStatus    status;
    
    CKvJSONRPCService *rpcserv;
    jsonrpc::HttpServer *httpserv;
    
    bool bExitFlag;
    
public:
    virtual bool Initialize(CThreadPool* pool, ILog *plogger);
    virtual bool Main();
    virtual bool Close();
    virtual bool SetIP( const char *ip );
    virtual void SetPort( unsigned short usport ) { port = usport;}
    
    unsigned short	GetPort() const{ return port; }
    const char*     GetIP() const {return listenIPv4;}
    ServerStatus GetStatus() const {return status;}
    
    CRPCServer();
    virtual ~CRPCServer();
    
};

}
#endif
