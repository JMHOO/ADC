#include <unistd.h>
#include <stdlib.h>									
#include <arpa/inet.h>
#include <stdio.h>
#include "TCPSyncIOServer.h"

namespace ADCS
{
    
    CTCPSIOServer::CTCPSIOServer(): socketid(0), threadPool(0), logger(0), port(0),
        iListenQueueLen(100),status(ServerStatus::Uninit)
    {
        memset( listenIPv4, 0, 16);
    }

   
    bool CTCPSIOServer::SetIP( const char *ip )
    {
        if( strlen(ip) >= 16 )
            return false;
        
        strcpy( listenIPv4, ip );
        return true;
    }
    
    bool CTCPSIOServer::Initialize(CThreadPool* pool, ILog *plogger)
    {
        if( status != ServerStatus::Uninit )
            return false;
        
        //struct protoent			*pProtocol;
        
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
        
        if( ServerAddr.sin_addr.s_addr == INADDR_NONE )
        {
            if(plogger)plogger->Error("TCP Server initialize failed: Invalid IP Address --- %s", listenIPv4);
            return false;
        }
        
        socketid = socket( AF_INET, SOCK_STREAM, 0 );
        if( socketid < 0 )
        {
            if(plogger)plogger->Error("TCP Server initialize failed: initialize socket failed.");
            return false;
        }
        
        int reuse_addr = 1;
        setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, (void*)(&(reuse_addr)), sizeof(reuse_addr));
        
        if( bind( socketid, (struct sockaddr *)(&ServerAddr), sizeof(struct sockaddr) ) == -1 )
        {
            close( socketid );
            if(plogger)plogger->Error("TCP Server initialize failed: bind socket failed.");
            return false;
        }
        
        if( listen( socketid, iListenQueueLen ) == -1 )
        {
            close( socketid );
            return false;
        }
        
        threadPool = pool;
        logger = plogger;
        
        status = ServerStatus::Inited;
        
        return true;
    }

  
    bool CTCPSIOServer::Main()
    {
        if( status != ServerStatus::Inited )
            return false;
        
        status = ServerStatus::Running;
        
        int	addrlen = sizeof(struct sockaddr_in);
        int	newSocket;
        
        CTCPSIOConnParam *pConnParam = NULL;
        
        while(1)
        {
            if( ( newSocket = accept( socketid, (struct sockaddr *)(&ClientAddr), (socklen_t *)&addrlen) ) == -1 )
            {
                if( logger )
                    logger->Error( "CTCPSIOServer::Main, accpet client socket failed --- socket id:%d, client ip:%s", socketid, inet_ntoa(ClientAddr.sin_addr) );
                continue;
            }
            
            if(logger)logger->Info("TCPServer::Main, new connection from client: %s, socket id:%d", inet_ntoa(ClientAddr.sin_addr), socketid);
            
            pConnParam = new CTCPSIOConnParam;
            if( !pConnParam )
            {
                if( logger )
                    logger->Error( "CTCPSIOServer::Main(): allocate memory for new connection failed, close socket." );
                close(newSocket);
                continue;
            }
            pConnParam->socketid = newSocket;
            pConnParam->ClientAddr = ClientAddr;
            
            if( threadPool->WakeUp( (void *)pConnParam) == false )
            {
                close(newSocket);
                delete pConnParam;
                
                if( logger )
                {
                    logger->Error( "CTCPSIOServer : Thread Pool wake up failed. Server main exiting. Socket:%d, client ip:%s", newSocket, inet_ntoa(ClientAddr.sin_addr) );
                }
                break;
            }
        }
        
        Clear();
        return true;
    }

    bool CTCPSIOServer::Close()
    {
        if( threadPool )
        {
            status = ServerStatus::Exiting;
            // we should need a client to connet server because ServerMain was blocked in accept()
            status = ServerStatus::Uninit;
        }
        
        return true;
    }

    bool CTCPSIOServer::Clear()
    {
        close(socketid);
        socketid = 0;
        threadPool = NULL;
        return true;
    }
}
