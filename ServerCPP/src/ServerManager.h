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
    int tcpport;
    int rpcport;
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
    bool  RegisterServer(int socketid, std::string serverAddr, int nTCPPort, int nRPCPort);
    bool  UnregisterServer(int socketid);
    
private:
    ServerMap               m_srvMap;
    ILog*                   m_logger;
    
    static CServerManager*  srvMgr_instance;
};


#endif
