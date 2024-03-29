#include <stdio.h>
#include <iostream>
#include "SrvApp.h"
#include "TCPSyncIOServer.h"
#include "UDPSyncIOServer.h"
#include "TCPSyncProcessor.h"
#include "UDPSyncProcessor.h"
#include "AgentServProcessor.h"
#include "KVServer.h"
#include "RPCServer.h"
#include "ServerManager.h"
#include "KVCoordinator.h"
#include "paxosInstance.h"

const char kvserver_cluster[INTERAL_SERVER_COUNT][16] = { "tcp", "udp", "rpc" };

bool CServerApp::Start(unsigned short usPort, std::string sMode,  std::string serverExternalAddr, std::string discoveryServerAddr, std::string simulation)
{
    m_runmode = sMode;
    m_serverExternalAddr = serverExternalAddr;
    m_discoverySrvAddr = discoveryServerAddr;
    
    if( sMode == "kvserver" )
    {
        // create 3-kinds server: tcp, udp, rpc
        for(int i = 0; i < INTERAL_SERVER_COUNT; i++ )
        {
            m_servers[i] = ADCS::IServer::CreateServer(kvserver_cluster[i]);
        }
        
        // start all server
        bool bAllServerStarted = true;
        for(int i = 0; i < INTERAL_SERVER_COUNT; i++ )
        {
            if( !m_servers[i]->Start(usPort+i) )
            {
                bAllServerStarted = false;
                std::cout << "Failed to start " << kvserver_cluster << " server, application exit." << endl;
                break;
            }
        }
        
        // some of server start failed, destroy all and exit.
        if( !bAllServerStarted )
        {
            for( int i = 0; i < INTERAL_SERVER_COUNT; i++ )
            {
                if( m_servers[i] )
                {
                    m_servers[i]->Stop();
                    delete m_servers[i];
                    m_servers[i] = nullptr;
                }
            }
        }
        
        // initialize KVServer
        if(!CKVServer::Create())
        {
            // .....
        }
        
        // initialize Coordinator
        if( !ADCS::CKvCoordinator::Create() )
        {
            // ......
        }
        
        // initialize paxos instance
        bool bSimulateionFailed = simulation == "true";
        if( !Paxos::Instance::Create(bSimulateionFailed) )
        {
            // ......
        }
        
        // Start discovery Client
        m_discoveryClient = new ADCS::CDiscoveryClient();
        if( m_discoveryClient->Start(m_discoverySrvAddr, m_serverExternalAddr, usPort, usPort + 2, usPort + 1))
        {
            std::cout << "Discovery Client is started. " << std::endl;
        }

    }
    else if (sMode == "discovery" )
    {
        // Start Discovery Server
        m_discoveryServer = ADCS::IServer::CreateServer("discovery");
        if(!m_discoveryServer->Start(15000))
        {
            return false;
        }
        
        // initialize Server Manager
        if( !CServerManager::Create( m_discoveryServer->GetLogger() ) )
        {
            //......
        }
    }
    
    
        return true;
}

bool CServerApp::Stop()
{
    if( m_runmode == "kvserver" )
    {
        for( int i = 0; i < INTERAL_SERVER_COUNT; i++ )
        {
            if( m_servers[i] )
            {
                m_servers[i]->Stop();
                delete m_servers[i];
                m_servers[i] = nullptr;
            }
        }
        
        CKVServer::Destory();
    }
    else if (m_runmode == "discovery")
    {
        if( m_discoveryServer )
        {
            m_discoveryServer->Stop();
            delete m_discoveryServer;
            m_discoveryServer = nullptr;
        }
        CServerManager::Destory();
        ADCS::CKvCoordinator::Destory();
    }
    return true;
}

void CServerApp::ListAllServer()
{
    if( m_runmode == "discovery" )
    {
        CServerManager* pSrvMgr = CServerManager::GetInstance();
        std::vector<PServerInfo> serverList = pSrvMgr->GetAliveServers();
        
        if( serverList.size() == 0 )
        {
            std::cout << "No Server Available." << endl;
        }
        
        for(size_t i = 0; i < serverList.size(); i++ )
        {
            std::cout << "Server: " << serverList[i]->serverAddr << "[tcp:" << serverList[i]->tcpport << ",rpc:" << serverList[i]->rpcport << "]" << endl;
        }
    }
    else if( m_runmode == "kvserver" )
    {
        ADCS::CKvCoordinator* coor = ADCS::CKvCoordinator::GetInstance();
        ADCS::ServerList serverList = coor->GetServerList();
        
        if( serverList.size() == 0 )
        {
            std::cout << "No Server Available." << endl;
        }
        
        for(size_t i = 0; i < serverList.size(); i++ )
        {
            std::cout << "Server: " << serverList[i].address << ":" << serverList[i].port << endl;
        }

    }
}


CServerApp::CServerApp()
{
    for( int i = 0; i < INTERAL_SERVER_COUNT; i++ )
    {
        m_servers[i] = nullptr;
    }
    m_discoveryServer = nullptr;
    
    m_runmode = "kvserver";
}

CServerApp::~CServerApp()
{
    
}
