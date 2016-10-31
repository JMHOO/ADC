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
    unsigned short	port;
    char            listenIPv4[16];
    ServerStatus    status;
    
    CKvJSONRPCService *rpcserv;
    jsonrpc::HttpServer *httpserv;
    
    pthread_mutex_t	Mutex;
    pthread_cond_t Condition;
    
    bool bExitFlag;
    
public:
    virtual bool Initialize();
    virtual bool Main();
    virtual bool Close();
    virtual bool SetIP( const char *ip );
    virtual void SetPort( unsigned short usport ) { port = usport;}
    virtual unsigned short GetPort(){ return port; }
    virtual const char* GetIP() {return listenIPv4;}

    CRPCServer(IExecuteor* executor);
    virtual ~CRPCServer();
 
    ServerStatus GetStatus() const {return status;}
    
};

}
#endif
