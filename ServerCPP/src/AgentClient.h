#ifndef __ADC_AgentClient_H__
#define __ADC_AgentClient_H__

#include <pthread.h>
#include <vector>
#include <string>
#include "IClient.h"
#include "TCPClient.h"
#include "GLog.h"
#include "PackageInterface.h"

using namespace std;

namespace ADCS
{
    typedef std::pair<string,int> ServerDesc;
    typedef std::vector<ServerDesc> ServerList;
    
    class CDiscoveryClient
    {
    private:
        static void* ClientMain( void* pParam );
        
    private:
        pthread_t   AgentClientMainThreadId;
        CTCPClient  m_tcpClient;
        ILog*       m_logger;
        std::string m_serverExternalIP;
        int         m_tcpserverPort;
        int         m_rpcserverPort;
        
        ServerList  m_aliveSrvList;
        bool __try_connect_agent_server();
        bool __do_send(IPacket* p);
        IPacket* __do_recv();
        
    public:
        bool Start(std::string strAgentSrvAddr, std::string strExternalIP, int nTCPServerPort, int nRPCServerPort );
        bool Stop();
        IClient* GetClient();
        
        CDiscoveryClient();
        ~CDiscoveryClient();
    };
}

#endif
