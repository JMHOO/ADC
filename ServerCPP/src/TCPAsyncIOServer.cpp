#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "TCPAsyncIOServer.h"


namespace ADCS{
    

    bool CTCPAsyncIOServer::SetIP( const char *ip )
    {
        if( strlen(ip) >= 16 )
            return false;
        
        strcpy( listenIPv4, ip );
        return true;
    }

    bool CTCPAsyncIOServer::Initialize(CThreadPool* pool, ILog *logger)
    {
        if( status != ServerStatus::Uninit )
            return false;
        
        struct sockaddr_in serverAddr;
        memset( &serverAddr, 0, sizeof(sockaddr_in) );
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons( port );
        serverAddr.sin_addr.s_addr = inet_addr( listenIPv4 );
        if( serverAddr.sin_addr.s_addr == INADDR_NONE )
            return false;
        
        socketid = socket( AF_INET, SOCK_STREAM, 0 );
        if( socketid < 0 )
        {
            return false;
        }
        
        int reuse_addr = 1;
        setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, (void*)(&(reuse_addr)), sizeof(reuse_addr));
        
        if( bind( socketid, (struct sockaddr *)(&serverAddr), sizeof(struct sockaddr) ) == -1 )
        {
            close( socketid );
            return false;
        }
        
        if( listen( socketid, iListenQueueLen ) == -1 )
        {
            close( socketid );
            return false;
        }
        
        threadPool = pool;
        logger = logger;
        
        status = ServerStatus::Inited;
        
        return true;
    }


    bool CTCPAsyncIOServer::ServerMain()
    {
        if( status != ServerStatus::Inited )
            return false;
        
        status = ServerStatus::Running;
        
        int	addrlen = sizeof(struct sockaddr_in);
        int	newSocket;
        
        struct sockaddr_in		sClientAddr;
        CTCPAIOConnParam		*pConnParam = NULL;
        
        while(1)
        {
            if( ( newSocket = accept( socketid, (struct sockaddr *)(&sClientAddr), (socklen_t *)&addrlen) ) == -1 )
            {
                if( logger )
                    logger->Error( "TCPAIOServer : Accepted failed." );
                continue;
            }
            
            pConnParam = new CTCPAIOConnParam;

            pConnParam->Server = this;
            pConnParam->socketid = newSocket;
            pConnParam->ClientIP = sClientAddr;
            pConnParam->Event = Events::EV_Connect;
            pConnParam->Context = NULL;
            
            pConnParam->InitAIOControlBlock( newSocket );
            
            if( !threadPool->WakeUp( (void *)pConnParam ))
            {
                close(newSocket);
                delete pConnParam;
                
                if( logger )
                {
                    logger->Error( "TCPAIOServer : Thread Pool wake up failed. Socket:%d, IP: %s", newSocket, inet_ntoa(sClientAddr.sin_addr) );
                }
                break;
            }
            
        }
        
        Clear();
        return true;
    }


    bool CTCPAsyncIOServer::Close()
    {
        if( threadPool )
        {
            status = ServerStatus::Exiting;
            
            WaitForExit();
            
            threadPool = NULL;
            status = ServerStatus::Uninit;
        }
        
        return true;
    }

    bool CTCPAsyncIOServer::Clear()
    {
        close( socketid );
        socketid = 0;
        return true;
    }

    void CTCPAsyncIOServer::WaitForExit()
    {
        struct timespec		ts;
        
        ts.tv_sec = 0;
        ts.tv_nsec = 100000000;			//-- One hundred Milliseconds.
        
        while(socketid)
            nanosleep( &ts, NULL );				//-- Wait for CTCPAioServer::Main() exiting.
    }
    
    bool CTCPAsyncIOServer::AddJobToThreadPool( const CTCPAIOConnParam* pConnParam)
    {
        if( !threadPool )
            return false;
        
        return (threadPool->WakeUp( (void *)pConnParam ));
    }
    
    
    
    

    void CTCPAIOConnParam::AIOCompletionHandler( sigval_t sigval )
    {
        CTCPAIOConnParam *pConnParam = (CTCPAIOConnParam *)sigval.sival_ptr;
        
        if( !pConnParam->Server->AddJobToThreadPool( pConnParam ) )
        {
            IExecuteor	*executor = pConnParam->Server->GetExecutor();
            if( executor )
            {
                pConnParam->Event = Events::EV_ThreadPoolError;
                
                executor->Execute((void *)pConnParam );
            }
        }
    }

    void CTCPAIOConnParam::InitAIOControlBlock( int iSocket )
    {
        AIOControlBlock.aio_fildes = iSocket;
        AIOControlBlock.aio_offset = 0;
        AIOControlBlock.aio_buf = NULL;
        AIOControlBlock.aio_nbytes = 0;
        AIOControlBlock.aio_reqprio = 0;
        AIOControlBlock.aio_sigevent.sigev_notify = SIGEV_THREAD;	//SIGEV_SIGNAL;
        AIOControlBlock.aio_sigevent.sigev_value.sival_ptr = this;
        AIOControlBlock.aio_sigevent.sigev_notify_function = CTCPAIOConnParam::AIOCompletionHandler;
        AIOControlBlock.aio_sigevent.sigev_notify_attributes = NULL;
        //AIOControlBlock.aio_sigevent._sigev_un._sigev_thread._function =
        //AIOControlBlock.aio_sigevent._sigev_un._sigev_thread._attribute = NULL;
    }
    
    inline void	CTCPAIOConnParam::SetAIOBuffer( void * lpBuffer, size_t BufferLen )
    {
        AIOControlBlock.aio_buf = lpBuffer;
        AIOControlBlock.aio_nbytes = BufferLen;
    }
    
    inline void	CTCPAIOConnParam::SetAIOCompletionHandler()
    {
        AIOControlBlock.aio_sigevent.sigev_notify_function = CTCPAIOConnParam::AIOCompletionHandler;
    }
    
    inline void CTCPAIOConnParam::CloseSession()
    {
        close( socketid );
        delete this;
    }
    
}
