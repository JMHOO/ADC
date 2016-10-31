#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "UDPSyncIOServer.h"
#include "UDPClient.h"

namespace ADCS{
    
    bool CUDPSyncIOServer::SetIP( const char *ip )
    {
        if( strlen(ip) >= 16 )
            return false;
        
        strcpy( listenIPv4, ip );
        return true;
    }
 
    bool CUDPSyncIOServer::Initialize()
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
            if(m_logger)m_logger->Error("%s Server initialize failed: initialize socket failed.", m_serverName.c_str());
            return false;
        }
        
        int reuse_addr = 1;
        setsockopt(socketid, SOL_SOCKET, SO_REUSEADDR, (char*)(&(reuse_addr)), sizeof(reuse_addr));
        
        if( bind( socketid, (struct sockaddr *)(&ServerAddr), sizeof(struct sockaddr) ) == -1 )
        {
            close( socketid );
            if(m_logger)m_logger->Error("%s Server initialize failed: bind socket failed.", m_serverName.c_str());
            return false;
        }
        
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
                if( m_logger )
                    m_logger->Error( "%s Server::Main(): allocate memory for new connection failed." , m_serverName.c_str());
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
        if( m_pThreadPool )
        {
            status = ServerStatus::Exiting;
            if(m_logger)m_logger->Warning("%s Server Closing...", m_serverName.c_str());
            if(m_logger)m_logger->Warning("Destory thread pool.");

            m_pThreadPool->Destory();
            
            if(m_logger)m_logger->Warning("Server closer working....");
            
            CUDPClient client;
            client.SetIP("127.0.0.1");
            client.SetPort(GetPort());
            client.Connect();
            
            char pBuffer[16];
            memset(pBuffer, 0, 16);
            client.SendInfo(pBuffer, 16);
            
            client.Close();
            status = ServerStatus::Uninit;
            if(m_logger)m_logger->Warning("%s server shutted down.", m_serverName.c_str());
        }
        
        return true;
    }

    bool CUDPSyncIOServer::Clear()
    {
        close( socketid );
        socketid = 0;
        
        status = ServerStatus::Uninit;
        
        return true;
    }

    bool CUDPSyncIOServer::AddJobToThreadPool( CUDPSyncIOConnParam * pCP)
    {
        bool bSuccess = m_pThreadPool->WakeUp((void *)pCP);
        if( !bSuccess && m_logger )
        {
            m_logger->Error("CUDPSyncIOServer::AddJobToThreadpool(): Thread wake up failed.");
        }
        
        return bSuccess;
    }
    
    void CUDPSyncIOServer::ClearSession( CUDPSyncIOConnParam * pCP )
    {
        if(pCP) delete pCP;
    }
    
    CUDPSyncIOServer::CUDPSyncIOServer(IExecuteor* executor): ADCS::IServer("UDP", executor), socketid(0), port(0), status(ServerStatus::Uninit)
    {
        memset( listenIPv4, 0, 16);
    }
    
    CUDPSyncIOServer::~CUDPSyncIOServer()
    {
        
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
