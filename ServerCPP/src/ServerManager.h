#ifndef _ADCS__ServerManager_H_
#define _ADCS__ServerManager_H_

#include <string>
#include <map>
#include <vector>
#include <pthread.h>
#include "ErrorCode.h"

typedef struct __server_info
{
    int socketid;
    int port;
    std::string serverAddr;
}ServerInfo, *PServerInfo;

typedef std::map<int, PServerInfo> ServerMap;

class CServerManager
{
public:
    static bool             Create();
    static void             Destory();
    static CServerManager*  GetInstance();
    
    CServerManager();
    ~CServerManager();
    
    std::vector<PServerInfo>  GetAliveServers();
    bool  RegisterServer(int socketid, std::string serverAddr, int port);
    bool  UnregisterServer(int socketid);
    
private:
    ServerMap                m_srvMap;
    
    static CServerManager*   srvMgr_instance;
};


#endif
