#include <stdio.h>
#include "ServerManager.h"

CServerManager* CServerManager::srvMgr_instance = nullptr;

bool CServerManager::Create()
{
    if( srvMgr_instance == nullptr )
    {
        srvMgr_instance = new CServerManager();
        if( srvMgr_instance == nullptr )
            return false;
    }
    return true;
}

void CServerManager::Destory()
{
    if(srvMgr_instance)
    {
        delete srvMgr_instance;
        srvMgr_instance = nullptr;
    }
}

CServerManager* CServerManager::GetInstance()
{
    return srvMgr_instance;
}

CServerManager::CServerManager()
{
}

CServerManager::~CServerManager()
{
}


std::vector<PServerInfo> CServerManager::GetAliveServers()
{
    std::vector<PServerInfo> srvList;
    for(ServerMap::iterator it = m_srvMap.begin(); it != m_srvMap.end(); it++ )
        srvList.push_back(it->second);
    
    return srvList;
}

bool CServerManager::RegisterServer(int socketid, std::string serverAddr, int port)
{
    ServerMap::iterator it = m_srvMap.find(socketid);
    if( it != m_srvMap.end())
    {
        it->second->serverAddr = serverAddr;
        it->second->port = port;
    }
    else
    {
        PServerInfo si = new ServerInfo();
        si->socketid = socketid;
        si->serverAddr = serverAddr;
        si->port = port;
        m_srvMap.insert(ServerMap::value_type(socketid, si));
    }
    return true;
}

bool CServerManager::UnregisterServer(int socketid)
{
    ServerMap::iterator it = m_srvMap.find(socketid);
    if( it != m_srvMap.end())
    {
        PServerInfo si = it->second;
        delete si;
        it = m_srvMap.erase(it);
        
        return true;
    }
    return false;
}

