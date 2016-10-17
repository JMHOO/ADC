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
        
        pthread_mutex_t	Mutex;
        int				socketid;
        
        CThreadPool     *threadPool;
        ILog            *logger;
        
        unsigned short	port;
        char            listenIPv4[16];
        
        ServerStatus    status;
        
        bool Clear();
        void ClearSession( CUDPSyncIOConnParam * pCP );
        bool AddJobToThreadPool( CUDPSyncIOConnParam * pCP);
        
    public:
        virtual bool	Initialize(CThreadPool* pool, ILog *plogger);
        virtual bool	Main();
        virtual bool	Close();
        virtual bool    SetIP( const char *ip );
        virtual void    SetPort( unsigned short usport ) { port = usport;}
        
        void SetListenQueueLen( int length ){ }
        
        CUDPSyncIOServer();
        virtual ~CUDPSyncIOServer();
        
        unsigned short	GetPort() const{ return port; }
        const char*     GetIP() const {return listenIPv4;}
        ServerStatus GetStatus() const {return status;}
    };
}

#endif
