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
    m_globalServerID = 0;
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

int CServerManager::RegisterServer(int socketid, std::string serverAddr, int nTCPPort, int nRPCPort, int nUDPPort)
{
    int nNewID = -1;
    if(m_logger)m_logger->Info("Discovery Server: registering kv server, tcp[%s:%d], rpc[%s:%d]...", serverAddr.c_str(), nTCPPort, serverAddr.c_str(), nRPCPort);
    ServerMap::iterator it = m_srvMap.find(socketid);
    if( it != m_srvMap.end())
    {
        if(m_logger)m_logger->Info("Discovery Server: found kv server, update [%s:%d/%d] --->>> [%s:%d/%d]",
                                   it->second->serverAddr.c_str(), it->second->tcpport, it->second->rpcport, serverAddr.c_str(), nTCPPort, nRPCPort);
        it->second->serverAddr = serverAddr;
        it->second->tcpport = nTCPPort;
        it->second->rpcport = nRPCPort;
        it->second->udpport = nUDPPort;
    }
    else
    {
        PServerInfo si = new ServerInfo();
        si->socketid = socketid;
        si->serverAddr = serverAddr;
        si->tcpport = nTCPPort;
        si->rpcport = nRPCPort;
        si->udpport = nUDPPort;
        m_srvMap.insert(ServerMap::value_type(socketid, si));
        nNewID = ++m_globalServerID;
        si->nodeid = nNewID;
        if(m_logger)m_logger->Info("Discovery Server: kv server[#%d], tcp[%s:%d], rpc[%s:%d] was registered.", nNewID, serverAddr.c_str(), nTCPPort, serverAddr.c_str(), nRPCPort);
    }
    return nNewID;
}

bool CServerManager::UnregisterServer(int socketid)
{
    ServerMap::iterator it = m_srvMap.find(socketid);
    if( it != m_srvMap.end())
    {
        if(m_logger)m_logger->Info("Discovery Server: kv server[%s:%d/%d] was Unregistered.", it->second->serverAddr.c_str(), it->second->tcpport, it->second->rpcport);

        PServerInfo si = it->second;
        delete si;
        it = m_srvMap.erase(it);
        
        
        return true;
    }
    return false;
}

