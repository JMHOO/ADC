#include <stdio.h>
#include <time.h>
#include "AgentClient.h"
#include "jsonAgentPackage.h"

namespace ADCS
{
    void* CDiscoveryClient::ClientMain( void* pParam )
    {
        CDiscoveryClient* pAgent = reinterpret_cast<CDiscoveryClient*>(pParam);
        ILog* plogger = pAgent->m_logger;
        
        // It will retry if server unreachable
        pAgent->__try_connect_agent_server();
        // register self first.
        jsonAgentPacket p;
        p.BuildRegisterRequest(pAgent->m_serverExternalIP, pAgent->m_tcpserverPort, pAgent->m_rpcserverPort);
        
        pAgent->__do_send(&p);
        IPacket* pRes = pAgent->__do_recv();
        if(pRes == NULL)
        {
            if(plogger)plogger->Error("Discovery Client: get register response failed.");
        }
        
        // every 15 seconds, thread wake up to get server list
        // in order to maintain the TCP connection, like heart beat package
        timespec ts;
        ts.tv_sec = 15;
        ts.tv_nsec = 0;
        
        while(true)
        {
            bool bSusscuss = false;
            // request online rpc server list
            p.BuildGetServerListRequest("rpc");
            bool bSent = pAgent->__do_send(&p);
            
            IPacket* pRes = pAgent->__do_recv();
            jsonAgentPacket* pSrvList = dynamic_cast<jsonAgentPacket*>(pRes);
            
            bSusscuss = bSent && pSrvList;
            
            if( pSrvList )
            {
                nlohmann::json jSrvList = pSrvList->GetServerList();
                if( jSrvList.is_array())
                {
                    // if(plogger)plogger->Info("Discovery Client: server list [%s]", jSrvList.dump().c_str());
                    pAgent->m_aliveSrvList.clear();
                    for(size_t i = 0; i < jSrvList.size(); i++)
                    {
                        ServerDesc sd;
                        sd.first = jSrvList[i]["address"];
                        sd.second = jSrvList[i]["port"];
                        
                        if( sd.first != pAgent->m_serverExternalIP && sd.second != pAgent->m_rpcserverPort)
                            pAgent->m_aliveSrvList.push_back(sd);
                    }
                }
            }
            
            nanosleep(&ts, NULL);
            
            if(!bSusscuss)
            {
                if(plogger)plogger->Warning("Discovery Client: it seems the connection was lost, reconnecting...");
                pAgent->__try_connect_agent_server();
            }
        }
        
        return 0;
    }
    
    bool CDiscoveryClient::__do_send(IPacket* p)
    {
        const char* ptrRequest = nullptr;
        unsigned long ulRequestLength = 0;
        p->ToBytes(ptrRequest, ulRequestLength);
        
        int nSentLen = m_tcpClient.SendInfo(ptrRequest, (int)ulRequestLength);
        if( (int)ulRequestLength != nSentLen )
        {
            if(m_logger)m_logger->Error("Discovery Client: sending register package error. Agent client exit. Expect to send %d bytes, in fact, it sent %d bytes", ulRequestLength, nSentLen);
            m_tcpClient.Close();
            return false;
        }
        return true;
    }
    
    IPacket* CDiscoveryClient::__do_recv()
    {
        char sBuffer[1024];
        memset(sBuffer, 0, 1024);
        ADCS::PACK_HEADER *ptrHeader = (ADCS::PACK_HEADER *)sBuffer;
        
        int nRecvLen = m_tcpClient.RecvInfo(sBuffer, (int)(sizeof(ADCS::PACK_HEADER)));
        
        if( (int)(sizeof(ADCS::PACK_HEADER)) > nRecvLen )
        {
            if( nRecvLen == 0 )
            {
                if(m_logger)m_logger->Info("Discovery Client: nothing received.");
                
            }
            else
            {
                if(m_logger)m_logger->Error("Discovery Client: receive package header error. Expect to receive %d bytes, in fact, it receive %d bytes.", sizeof(ADCS::PACK_HEADER), nRecvLen);
            }
            
            return NULL;
        }
        
        int nRestDataLen = ntohl(ptrHeader->Length) - nRecvLen;
        nRecvLen = m_tcpClient.RecvInfo(sBuffer + nRecvLen, nRestDataLen );
        if( nRecvLen != nRestDataLen )
        {
            if(m_logger)m_logger->Error("Discovery Client: receive package payload error. Expect to receive %d bytes, in fact, it receive %d bytes -- %s", nRestDataLen, nRecvLen, sBuffer+sizeof(ADCS::PACK_HEADER));
            return NULL;
        }
        
        return IPacket::CreatePackage(sBuffer, ntohl(ptrHeader->Length), m_tcpClient.GetSocketID());
    }
    
    CDiscoveryClient::CDiscoveryClient()
    {
        m_logger = new GlobalLog("discovery", LL_DEBUG);
    }
    
    CDiscoveryClient::~CDiscoveryClient()
    {
        delete m_logger;
    }

    IClient* CDiscoveryClient::GetClient()
    {
        return &m_tcpClient;
    }
    
    bool CDiscoveryClient::__try_connect_agent_server()
    {
        int nRetryCount = 0;
        
        if(m_logger)m_logger->Info("Discovery Client: Trying to connect agent server: %s:%d", m_tcpClient.GetIPAddr(), m_tcpClient.GetPort());
        
        do
        {
            if( nRetryCount >= 100) // 50 minutes later, abort it
                break;
            
            if( m_tcpClient.Connect() )
            {
                if(m_logger)m_logger->Info("Discovery Client: Server connected.");
                return true;
            }
            else
            {
                if(m_logger)m_logger->Warning("Discovery Client: Connect to server failed, will retry in 30 seconds...");
                timespec ts;
                ts.tv_sec = 30;
                ts.tv_nsec = 0;
                nanosleep(&ts, NULL);
                nRetryCount++;
            }

        }while(true);
        
        return false;
    }
    
    bool CDiscoveryClient::Start(std::string strAgentSrvAddr, std::string strExternalIP, int nTCPServerPort, int nRPCServerPort)
    {
        m_serverExternalIP = strExternalIP;
        m_tcpserverPort = nTCPServerPort;
        m_rpcserverPort = nRPCServerPort;
        
        bool bStarted = false;
        m_tcpClient.SetIP(strAgentSrvAddr.c_str());
        m_tcpClient.SetPort(15000);
        m_tcpClient.SetTimeOut(10);
        m_tcpClient.SetRetryTime(3);
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
        
        if( pthread_create( &AgentClientMainThreadId, &attr, ClientMain, (void *)this ) != 0 )
        {
            bStarted = false;
        }
        else
            bStarted = true;
        
        pthread_attr_destroy(&attr);
        
        if(m_logger)m_logger->Info("Discovery Client is started.");
        
        return bStarted;
    }
    
    bool CDiscoveryClient::Stop()
    {
        pthread_join( AgentClientMainThreadId, NULL );
        
        return true;
    }

}
