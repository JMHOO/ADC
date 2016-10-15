#ifndef _ADCS__TCP_SYNCIO_SERVER_H_
#define _ADCS__TCP_SYNCIO_SERVER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include "GLog.h"
#include "Network.h"
#include "ThreadPool.h"
#include "ServerInterface.h"

namespace ADCS
{
    struct CTCPSIOConnParam
    {
        int	socketid;
        struct sockaddr_in ClientAddr;
    };

    class CTCPSIOServer: public IServer
    {
        int	socketid;
        struct sockaddr_in ServerAddr;
        struct sockaddr_in ClientAddr;

        CThreadPool     *threadPool;
        ILog            *logger;
        unsigned short	port;
        char            listenIPv4[16];
        ServerStatus    status;
        int             iListenQueueLen;
        
        bool Clear();
        
    public:
        virtual bool	Initialize(CThreadPool* pool, ILog *plogger);
        virtual bool	Main();
        virtual bool	Close();
        virtual bool    SetIP( const char *ip );
        virtual void    SetPort( unsigned short usport ) { port = usport;}
        
        const char*     GetIP() const {return listenIPv4;}
        unsigned short  GetPort() const {return port;}
        void            SetListenQueueLen( int length ){iListenQueueLen = length;}
        ServerStatus    GetStatus() const {return status;}
        
        CTCPSIOServer();
        virtual ~CTCPSIOServer() {}
    };
}

#endif
