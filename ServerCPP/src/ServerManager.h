#ifndef _ADCS__ServerManager_H_
#define _ADCS__ServerManager_H_

#include <string>
#include <map>
#include <vector>
#include <pthread.h>
#include "ErrorCode.h"
#include "GLog.h"

typedef struct __server_info
{
    int socketid;
    int nodeid;
    int tcpport;
    int rpcport;
    int udpport;
    std::string serverAddr;
}ServerInfo, *PServerInfo;


typedef std::map<int, PServerInfo> ServerMap;

class CServerManager
{
public:
    static bool             Create(ILog* logger = NULL);
    static void             Destory();
    static CServerManager*   GetInstance();
    
    CServerManager(ILog* logger = NULL);
    ~CServerManager();
    
    std::vector<PServerInfo>  GetAliveServers();
    int  RegisterServer(int socketid, std::string serverAddr, int nTCPPort, int nRPCPort, int nUDPPort);
    bool  UnregisterServer(int socketid);
    
private:
    ServerMap               m_srvMap;
    ILog*                   m_logger;
    
    int                     m_globalServerID;
    static CServerManager*  srvMgr_instance;
};


#endif
