#include <unistd.h>
#include <stdlib.h>									
#include <arpa/inet.h>
#include <stdio.h>
#include "TCPSyncIOServer.h"
#include "TCPClient.h"

namespace ADCS
{
    
    CTCPSIOServer::CTCPSIOServer(const char* sName, IExecuteor* executor): ADCS::IServer(sName, executor), socketid(0), port(0),
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
    
    bool CTCPSIOServer::Initialize()
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
            if(m_logger)m_logger->Error("%s Server initialize failed: Invalid IP Address --- %s", m_serverName.c_str(), listenIPv4);
            return false;
        }
        
        socketid = socket( AF_INET, SOCK_STREAM, 0 );
        if( socketid < 0 )
        {
            if(m_logger)m_logger->Error("%s Server initialize failed: initialize socket failed.", m_serverName.c_str());
            return false;
        }
        
        int reuse_addr = 1;
        setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, (void*)(&(reuse_addr)), sizeof(reuse_addr));
        
        if( bind( socketid, (struct sockaddr *)(&ServerAddr), sizeof(struct sockaddr) ) == -1 )
        {
            close( socketid );
            if(m_logger)m_logger->Error("%s Server initialize failed: bind socket failed.", m_serverName.c_str());
            return false;
        }
        
        if( listen( socketid, iListenQueueLen ) == -1 )
        {
            close( socketid );
            return false;
        }
        
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
                if( m_logger )
                    m_logger->Error( "%s Server::Main, accpet client socket failed --- socket id:%d, client ip:%s", m_serverName.c_str(), socketid, inet_ntoa(ClientAddr.sin_addr) );
                continue;
            }
            
            if(m_logger)m_logger->Info("%s Server::Main, new connection from client: %s, socket id:%d", m_serverName.c_str(), inet_ntoa(ClientAddr.sin_addr), socketid);
            
            pConnParam = new CTCPSIOConnParam;
            if( !pConnParam )
            {
                if( m_logger )
                    m_logger->Error( "%s Server::Main(): allocate memory for new connection failed, close socket." );
                close(newSocket);
                continue;
            }
            pConnParam->socketid = newSocket;
            pConnParam->ClientAddr = ClientAddr;
            
            if( m_pThreadPool->WakeUp( (void *)pConnParam) == false )
            {
                close(newSocket);
                delete pConnParam;
                
                if( m_logger )
                {
                    m_logger->Error( "%s Server : Thread Pool wake up failed. Server main exiting. Socket:%d, client ip:%s", m_serverName.c_str(), newSocket, inet_ntoa(ClientAddr.sin_addr) );
                }
                break;
            }
        }
        
        Clear();
        return true;
    }

    bool CTCPSIOServer::Close()
    {
        if( m_pThreadPool )
        {
            status = ServerStatus::Exiting;
            if(m_logger)m_logger->Warning("%s Server Closing...", m_serverName.c_str());
            if(m_logger)m_logger->Warning("Destory thread pool.");

            m_pThreadPool->Destory();
            
            if(m_logger)m_logger->Warning("Server closer working....");
            // we should need a client to connet server because ServerMain was blocked in accept()
            CTCPClient client;
            client.SetIP("127.0.0.1");
            client.SetPort(GetPort());
            client.Connect();
            
            //char pBuffer[16];
            //client.SendInfo(pBuffer, 16);
            
            client.Close();
            
            status = ServerStatus::Uninit;
            if(m_logger)m_logger->Warning("%s server shutted down.", m_serverName.c_str());
        }
        
        return true;
    }

    bool CTCPSIOServer::Clear()
    {
        close(socketid);
        socketid = 0;
        return true;
    }
}
