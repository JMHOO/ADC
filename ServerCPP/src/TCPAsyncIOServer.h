#ifndef _ADCS__TCP_SYNCIO_SERVER_H_
#define _ADCS__TCP_SYNCIO_SERVER_H_

#include <aio.h>
#include <signal.h>
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
    #ifdef __APPLE__
    typedef union sigval sigval_t;
    #endif
 
    class CTCPAsyncIOServer;
    
    class CTCPAIOConnParam
    {
    public:
        struct aiocb AIOControlBlock;
        
    private:
        friend class CTCPAsyncIOServer;
        
        CTCPAsyncIOServer		*Server;
        
        static void				AIOCompletionHandler( sigval_t sigval );
        
    public:
        int						socketid;
        struct sockaddr_in		ClientIP;
        int						Event;
        void *					Context;
        
        CTCPAIOConnParam(): Server(NULL), socketid(0), Event(Events::EV_Connect), Context(0)
        {
        }
        
    private:
        void			InitAIOControlBlock( int iSocket );
        void			SetAIOCompletionHandler();
    public:
        void			SetAIOBuffer( void * lpBuffer, size_t BufferLen );
        void			CloseSession();
    };
    
    typedef CTCPAIOConnParam		TCPConnParam;
    

    class CTCPAsyncIOServer : public IServer
    {
        friend class CTCPAIOConnParam;
        
        int					socketid;
        CThreadPool         *threadPool;
        ILog				*logger;
        
        unsigned short		port;
        int					iListenQueueLen;
        char				listenIPv4[16];      // IPv4 xxx.xxx.xxx.xxx
        
        ServerStatus        status;
        
        bool			Clear();
        void			WaitForExit();
        bool			AddJobToThreadPool( const CTCPAIOConnParam* pConnParam);
        
    public:
        // from IServer interface
        virtual bool	Initialize(CThreadPool* pool, ILog *logger);
        virtual bool	Main();
        virtual bool	Close();
        
        const char*	GetIP() const {return listenIPv4;}
        bool        SetIP( const char *ip );
 
        unsigned short	GetPort() const {return port;}
        void	SetPort( unsigned short usport ) { port = usport;}
        void	SetListenQueueLen( int length ){iListenQueueLen = length;}
        ServerStatus GetStatus() const {return status;}
        
        ADCS::IExecuteor* GetExecutor()
        {
            if( threadPool )
                return threadPool->GetExecutor();
            return NULL;
        }

        CTCPAsyncIOServer(): socketid(0), threadPool(NULL), logger(NULL), port(0), iListenQueueLen(100), status(ServerStatus::Uninit)
        {
            memset( listenIPv4, 0, 16);
        }

    };
}
#endif
