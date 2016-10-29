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

bool CServerApp::Start(unsigned short usPort, std::string sMode, std::string sAddr)
{
    m_runmode = sMode;
    m_serverAddr = sAddr;
    if( sMode == "regular" )
    {
        // Start TCP Server
        //m_tcpProcessor = new TCPServerProcessor(m_pTcpLogger);
        m_tcpProcessor = new TCPSyncServerProcessor(m_pTcpLogger);
        m_pTcpThreadPool = new ADCS::CThreadPool();
        m_pTcpThreadPool->Init(*m_tcpProcessor, 10, 20, 100);
        
        //m_tcpServer = new ADCS::CTCPAsyncIOServer();
        m_tcpServer = new ADCS::CTCPSIOServer();
        m_tcpServer->SetIP("0.0.0.0");
        m_tcpServer->SetPort(usPort);
        if(m_tcpServer->Start(m_pTcpThreadPool, m_pTcpLogger))
        {
            //ADCS::CTCPAsyncIOServer* tcpserver = dynamic_cast<ADCS::CTCPAsyncIOServer*>(m_tcpServer);
            ADCS::CTCPSIOServer* tcpserver = dynamic_cast<ADCS::CTCPSIOServer*>(m_tcpServer);
            std::cout << "TCP server is started, "<< tcpserver->GetIP() << ":"<< tcpserver->GetPort()<<" LISTENING"<< std::endl;
            if( m_pTcpLogger)m_pTcpLogger->Info("TCP Server started: %s:%d LISTENING.", tcpserver->GetIP(), tcpserver->GetPort());
            
        }
        else
        {
            return false;
        }
        
        // Start UDP Server
        m_udpProcessor = new UDPServerProcessor(m_pUdpLogger);
        m_pUdpThreadPool = new ADCS::CThreadPool();
        m_pUdpThreadPool->Init(*m_udpProcessor, 10, 20, 100);
        
        m_udpServer = new ADCS::CUDPSyncIOServer();
        m_udpServer->SetIP("0.0.0.0");
        m_udpServer->SetPort(usPort+1);
        if( m_udpServer->Start(m_pUdpThreadPool, m_pUdpLogger))
        {
            ADCS::CUDPSyncIOServer* udpserver = dynamic_cast<ADCS::CUDPSyncIOServer*>(m_udpServer);
            std::cout << "UDP server is started, "<< udpserver->GetIP() << ":"<< udpserver->GetPort()<< std::endl;
            if( m_pUdpLogger)m_pUdpLogger->Info("UDP Server started: %s:%d LISTEN.", udpserver->GetIP(), udpserver->GetPort());
            
        }
        else
        {
            return false;
        }
        
        // Start RPC Server
        m_rpcServer = new ADCS::CRPCServer();
        m_rpcServer->SetIP("0.0.0.0");
        m_rpcServer->SetPort(usPort+2);
        if(m_rpcServer->Start(NULL, m_rpcLogger))
        {
            ADCS::CRPCServer* rpcserver = dynamic_cast<ADCS::CRPCServer*>(m_rpcServer);
            std::cout << "RPC server is started, "<< rpcserver->GetIP() << ":"<< rpcserver->GetPort()<<" LISTENING"<< std::endl;
            if( m_pUdpLogger)m_rpcLogger->Info("RPC Server started: %s:%d LISTEN.", rpcserver->GetIP(), rpcserver->GetPort());

        }
        else
        {
            return false;
        }
        
        // initialize KVServer
        if(!CKVServer::Create())
        {
            //.....
        }

    }
    else if (sMode == "agent" )
    {
        // Start Agent Server
        m_agentProcessor = new AgentServerProcessor(m_agentLogger);
        m_agentThreadPool = new ADCS::CThreadPool();
        m_agentThreadPool->Init(*m_agentProcessor, 10, 20, 100);

        m_agentServer = new ADCS::CTCPSIOServer();
        m_agentServer->SetIP("0.0.0.0");
        m_agentServer->SetPort(15000);
        if(m_agentServer->Start(m_agentThreadPool, m_agentLogger))
        {
            ADCS::CTCPSIOServer* agentserv = dynamic_cast<ADCS::CTCPSIOServer*>(m_agentServer);
            std::cout << "Agent server is started, "<< agentserv->GetIP() << ":"<< agentserv->GetPort()<<" LISTENING"<< std::endl;
            if( m_pTcpLogger)m_pTcpLogger->Info("Agent Server started: %s:%d LISTENING.", agentserv->GetIP(), agentserv->GetPort());
            
        }
        else
        {
            return false;
        }
        
        
        // initialize Server Manager
        if(!CServerManager::Create())
        {
            //......
        }
    }
    
    
        return true;
}

bool CServerApp::Stop()
{
    if( m_runmode == "regular" )
    {
        if( m_tcpServer)
        {
            m_tcpServer->Stop();
            delete m_tcpServer;
            m_tcpServer = NULL;
        }
        
        if( m_tcpProcessor)
        {
            delete m_tcpProcessor;
            m_tcpProcessor = NULL;
        }
        
        if( m_pTcpThreadPool)
        {
            m_pTcpThreadPool->Destory();
            delete m_pTcpThreadPool;
            m_pTcpThreadPool = NULL;
        }
        
        if( m_udpServer)
        {
            m_udpServer->Stop();
            delete m_udpServer;
            m_udpServer = NULL;
        }
        
        if( m_udpProcessor)
        {
            delete m_udpProcessor;
            m_udpProcessor = NULL;
        }
        
        if( m_pUdpThreadPool)
        {
            m_pUdpThreadPool->Destory();
            delete m_pUdpThreadPool;
            m_pUdpThreadPool = NULL;
        }
        
        
        CKVServer::Destory();
    }
    else if (m_runmode == "agent")
    {
        if( m_agentProcessor )
        {
            delete m_agentProcessor;
            m_agentProcessor = NULL;
        }
        
        if( m_agentThreadPool)
        {
            m_agentThreadPool->Destory();
            delete m_agentThreadPool;
            m_agentThreadPool = NULL;
        }

        CServerManager::Destory();
    }
    return true;
}


CServerApp::CServerApp(): m_tcpServer(NULL), m_pTcpThreadPool(NULL), m_tcpProcessor(NULL),m_pTcpLogger(NULL),
m_udpServer(NULL), m_pUdpThreadPool(NULL), m_udpProcessor(NULL), m_pUdpLogger(NULL), m_rpcServer(NULL), m_rpcLogger(NULL),
m_agentServer(NULL), m_agentThreadPool(NULL), m_agentProcessor(NULL)
{
    m_pTcpLogger = new GlobalLog("TCP", LL_DEBUG);
    m_pUdpLogger = new GlobalLog("UDP", LL_DEBUG);
    m_rpcLogger = new GlobalLog("RPC", LL_DEBUG);
    m_agentLogger = new GlobalLog("Agent", LL_DEBUG);
    m_runmode = "regular";
}

CServerApp::~CServerApp()
{
    delete m_pTcpLogger;
    delete m_pUdpLogger;
    delete m_rpcLogger;
    delete m_agentLogger;
}
