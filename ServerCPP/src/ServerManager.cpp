#include <stdio.h>
#include "ServerManager.h"

CServerManager* CServerManager::srvMgr_instance = nullptr;

bool CServerManager::Create(ILog* logger)
{
    if( srvMgr_instance == nullptr )
    {
        srvMgr_instance = new CServerManager(logger);
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

CServerManager::CServerManager(ILog* logger)
{
    m_logger = logger;
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
    if(m_logger)m_logger->Info("Discovery Server: registering kv server[%s:%d]...", serverAddr.c_str(), port);
    ServerMap::iterator it = m_srvMap.find(socketid);
    if( it != m_srvMap.end())
    {
        if(m_logger)m_logger->Info("Discovery Server: found kv server, update [%s:%d] --->>> [%s:%d]", it->second->serverAddr.c_str(), it->second->port, serverAddr.c_str(), port);
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
        if(m_logger)m_logger->Info("Discovery Server: kv server[%s:%d] was registered.", serverAddr.c_str(), port);
    }
    return true;
}

bool CServerManager::UnregisterServer(int socketid)
{
    ServerMap::iterator it = m_srvMap.find(socketid);
    if( it != m_srvMap.end())
    {
        if(m_logger)m_logger->Info("Discovery Server: kv server[%s:%d] was Unregistered.", it->second->serverAddr.c_str(), it->second->port);

        PServerInfo si = it->second;
        delete si;
        it = m_srvMap.erase(it);
        
        
        return true;
    }
    return false;
}

