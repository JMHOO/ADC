#include <stdio.h>
#include <time.h>
#include "AgentClient.h"
#include "jsonAgentPackage.h"

namespace ADCS
{
    void* CAgentClient::ClientMain( void* pParam )
    {
        CAgentClient* pAgent = reinterpret_cast<CAgentClient*>(pParam);
        ILog* plogger = pAgent->m_logger;
        
        // It will retry if server unreachable
        pAgent->__try_connect_agent_server();
        // register self first.
        jsonAgentPacket p;
        p.BuildRequest("register", pAgent->m_tcpserverIP, pAgent->m_tcpserverPort);
        
        pAgent->__do_send(&p);
        IPacket* pRes = pAgent->__do_recv();
        if(pRes == NULL)
        {
            if(plogger)plogger->Error("Agent Client: get register response failed.");
        }
        
        timespec ts;
        ts.tv_sec = 30;     // every seconds, thread wake up to get server list
        ts.tv_nsec = 0;
        
        while(true)
        {
            bool bSusscuss = false;
            // request online server list
            p.BuildRequest("getserverlist");
            bool bSent = pAgent->__do_send(&p);
            
            IPacket* pRes = pAgent->__do_recv();
            jsonAgentPacket* pSrvList = dynamic_cast<jsonAgentPacket*>(pRes);
            
            bSusscuss = bSent && pSrvList;
            
            if( pSrvList )
            {
                nlohmann::json jSrvList = pSrvList->GetServerList();
                if( jSrvList.is_array())
                {
                    pAgent->m_aliveSrvList.clear();
                    for(size_t i = 0; i < jSrvList.size(); i++)
                    {
                        ServerDesc sd;
                        sd.first = jSrvList[i]["address"];
                        sd.second = jSrvList[i]["port"];
                        
                        if( sd.first != pAgent->m_tcpserverIP && sd.second != pAgent->m_tcpserverPort)
                            pAgent->m_aliveSrvList.push_back(sd);
                    }
                }
            }
            
            nanosleep(&ts, NULL);
            
            if(!bSusscuss)
            {
                if(plogger)plogger->Warning("Agent Client: it seems the connection was lost, reconnecting...");
                pAgent->__try_connect_agent_server();
            }
            
            
            
        }
        
        return 0;
    }
    
    bool CAgentClient::__do_send(IPacket* p)
    {
        const char* ptrRequest = nullptr;
        unsigned long ulRequestLength = 0;
        p->ToBytes(ptrRequest, ulRequestLength);
        
        int nSentLen = m_tcpClient.SendInfo(ptrRequest, (int)ulRequestLength);
        if( (int)ulRequestLength != nSentLen )
        {
            if(m_logger)m_logger->Error("Agent Clent: sending register package error. Agent client exit. Expect to send %d bytes, in fact, it sent %d bytes", ulRequestLength, nSentLen);
            m_tcpClient.Close();
            return false;
        }
        return true;
    }
    
    IPacket* CAgentClient::__do_recv()
    {
        char sBuffer[1024];
        memset(sBuffer, 0, 1024);
        ADCS::PACK_HEADER *ptrHeader = (ADCS::PACK_HEADER *)sBuffer;
        
        int nRecvLen = m_tcpClient.RecvInfo(sBuffer, (int)(sizeof(ADCS::PACK_HEADER)));
        
        if( (int)(sizeof(ADCS::PACK_HEADER)) > nRecvLen )
        {
            if( nRecvLen == 0 )
            {
                if(m_logger)m_logger->Info("Agent Client: nothing received.");
                
            }
            else
            {
                if(m_logger)m_logger->Error("Agent Client: receive package header error. Expect to receive %d bytes, in fact, it receive %d bytes.", sizeof(ADCS::PACK_HEADER), nRecvLen);
            }
            
            return NULL;
        }
        
        int nRestDataLen = ntohl(ptrHeader->Length) - nRecvLen;
        nRecvLen = m_tcpClient.RecvInfo(sBuffer + nRecvLen, nRestDataLen );
        if( nRecvLen != nRestDataLen )
        {
            if(m_logger)m_logger->Error("Agent Client: receive package payload error. Expect to receive %d bytes, in fact, it receive %d bytes -- %s", nRestDataLen, nRecvLen, sBuffer+sizeof(ADCS::PACK_HEADER));
            return NULL;
        }
        
        return IPacket::CreatePackage(sBuffer, ntohl(ptrHeader->Length), m_tcpClient.GetSocketID());
    }
    
    CAgentClient::CAgentClient()
    {
        m_logger = NULL;
    }
    
    CAgentClient::~CAgentClient()
    {
        
    }

    IClient* CAgentClient::GetClient()
    {
        return &m_tcpClient;
    }
    
    bool CAgentClient::__try_connect_agent_server()
    {
        int nRetryCount = 0;
        
        if(m_logger)m_logger->Info("Agent Client: Trying to connect agent server: %s:%d", m_tcpClient.GetIPAddr(), m_tcpClient.GetPort());
        
        do
        {
            if( nRetryCount >= 100) // 50 minutes later, abort it
                break;
            
            if( m_tcpClient.Connect() )
            {
                if(m_logger)m_logger->Info("Agent Client: Server connected.");
                return true;
            }
            else
            {
                if(m_logger)m_logger->Warning("Agent Client: Connect to server failed, retry in 30 seconds...");
                timespec ts;
                ts.tv_sec = 30;
                ts.tv_nsec = 0;
                nanosleep(&ts, NULL);
                nRetryCount++;
            }

        }while(true);
        
        return false;
    }
    
    bool CAgentClient::Start(ILog * pLogger, std::string strAgentSrvAddr, std::string strTCPServerAddr, int nTCPServerPort)
    {
        m_tcpserverIP = strTCPServerAddr;
        m_tcpserverPort = nTCPServerPort;
        m_logger = pLogger;
        
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
        
        return bStarted;
    }
    
    bool CAgentClient::Stop()
    {
        pthread_join( AgentClientMainThreadId, NULL );
        
        return true;
    }

}
