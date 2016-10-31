#ifndef _ADCS__UDP_SYNCIO_SERVER_H_
#define _ADCS__UDP_SYNCIO_SERVER_H_


#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include "Network.h"
#include "GLog.h"
#include "ServerInterface.h"
#include "ThreadPool.h"

namespace ADCS
{
    const unsigned int G_MAX_UDP_BUFFER_SIZE = 2048;
    
    class CUDPSyncIOServer;
    
    class CUDPSyncIOConnParam
    {
        friend class CUDPSyncIOServer;
        
        CUDPSyncIOServer* Server;
        int	Recv();
        
    public:
        int                 socketid;
        struct sockaddr_in	ClientIP;
        int					BytesTransferred;
        char				Buffer[G_MAX_UDP_BUFFER_SIZE];
        
        CUDPSyncIOConnParam(): Server(NULL), socketid(0), BytesTransferred(0) { }
        int     Send(const char* sInfo, int nLen);
        void    CloseSession();
    };
    
    class CUDPSyncIOServer: public IServer
    {
        friend class CUDPSyncIOConnParam;
        
        int				socketid;
               
        unsigned short	port;
        char            listenIPv4[16];
        
        ServerStatus    status;
        
        bool Clear();
        void ClearSession( CUDPSyncIOConnParam * pCP );
        bool AddJobToThreadPool( CUDPSyncIOConnParam * pCP);
        
    public:
        virtual bool	Initialize();
        virtual bool	Main();
        virtual bool	Close();
        virtual bool SetIP( const char *ip );
        virtual void SetPort( const unsigned short usport ) { port = usport;}
        virtual const char* GetIP(){return listenIPv4;}
        virtual unsigned short GetPort(){ return port; }
        
        
        CUDPSyncIOServer(IExecuteor* executor);
        virtual ~CUDPSyncIOServer();
        
        ServerStatus GetStatus() const {return status;}
        void SetListenQueueLen( int length ){ }
        
    };
}

#endif
