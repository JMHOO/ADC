#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "UDPSyncIOServer.h"

namespace ADCS{
    
    bool CUDPSyncIOServer::SetIP( const char *ip )
    {
        if( strlen(ip) >= 16 )
            return false;
        
        strcpy( listenIPv4, ip );
        return true;
    }
 
    bool CUDPSyncIOServer::Initialize(CThreadPool* pool, ILog *plogger)
    {
        if( status != ServerStatus::Uninit )
            return false;
        
        struct sockaddr_in ServerAddr;
        
        memset( &ServerAddr, 0, sizeof(ServerAddr) );
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons( port );
        
        if( strlen( listenIPv4 ) >= 7)  //0.0.0.0
        {
            ServerAddr.sin_addr.s_addr = inet_addr( listenIPv4 );
        }
        else
        {
            ServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );
        }
       
        socketid = socket( AF_INET, SOCK_DGRAM, 0 );
        if( socketid < 0 )
        {
            if(plogger)plogger->Error("UDP Server initialize failed: initialize socket failed.");
            return false;
        }
        
        int reuse_addr = 1;
        setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, (char*)(&(reuse_addr)), sizeof(reuse_addr));
        
        if( bind( socketid, (struct sockaddr *)(&ServerAddr), sizeof(struct sockaddr) ) == -1 )
        {
            close( socketid );
            if(plogger)plogger->Error("UDP Server initialize failed: bind socket failed.");
            return false;
        }
        
        threadPool = pool;
        logger = plogger;
        
        status = ServerStatus::Inited;
        
        return true;
    }

    bool CUDPSyncIOServer::Main()
    {
        if( status != ServerStatus::Inited )
            return false;
        
        status = ServerStatus::Running;
        
        CUDPSyncIOConnParam *pConnParam = NULL;
      
        while(true)
        {
            pConnParam = new CUDPSyncIOConnParam;
            if( !pConnParam )
            {
                if( logger )
                    logger->Error( "CUDPSyncIOServer::Main(): allocate memory for new connection failed." );
                continue;
            }
            
            pConnParam->socketid = this->socketid;
            pConnParam->Server = this;
            
            do
            {
                pConnParam->Recv();
            }
            while( pConnParam->BytesTransferred <= 0);
                
            // add job to thread pool, if thread pool is exiting, abort it.
            if( !AddJobToThreadPool( pConnParam ) )
            {
                ClearSession(pConnParam);
                goto Exiting;
            }
        }
        
    Exiting:
        Clear();
        return true;
    }

   
 
    bool CUDPSyncIOServer::Close()
    {
        if( threadPool )
        {
            status = ServerStatus::Exiting;
            
            threadPool = NULL;
            
            status = ServerStatus::Uninit;
        }
        
        return true;
    }

    bool CUDPSyncIOServer::Clear()
    {
        close( socketid );
        socketid = 0;
        threadPool = NULL;
        
        status = ServerStatus::Uninit;
        
        return true;
    }

    bool CUDPSyncIOServer::AddJobToThreadPool( CUDPSyncIOConnParam * pCP)
    {
        bool bSuccess = threadPool->WakeUp((void *)pCP);
        if( !bSuccess && logger )
        {
            logger->Error("CUDPSyncIOServer::AddJobToThreadpool(): Thread wake up failed.");
        }
        
        return bSuccess;
    }
    
    void CUDPSyncIOServer::ClearSession( CUDPSyncIOConnParam * pCP )
    {
        if(pCP) delete pCP;
    }
    
    CUDPSyncIOServer::CUDPSyncIOServer(): socketid(0), threadPool(0), logger(0), port(0), status(ServerStatus::Uninit)
    {
        memset( listenIPv4, 0, 16);
    }
    
    CUDPSyncIOServer::~CUDPSyncIOServer()
    {
        pthread_mutex_destroy(&Mutex);
    }

    


    
    
    
    int	CUDPSyncIOConnParam::Recv()
    {
        socklen_t nSourceLen = sizeof(ClientIP);
        return	(BytesTransferred = (int)recvfrom(socketid, Buffer, G_MAX_UDP_BUFFER_SIZE, 0,
            (struct sockaddr *)&ClientIP, &nSourceLen ));
    }
    
    int CUDPSyncIOConnParam::Send(const char* sInfo, int nLen )
    {
        return (BytesTransferred = (int)sendto(socketid, sInfo, nLen, 0, (struct sockaddr *)&ClientIP, sizeof(ClientIP) ));
    }
    
    void CUDPSyncIOConnParam::CloseSession()
    {
        Server->ClearSession(this);
    }
}
