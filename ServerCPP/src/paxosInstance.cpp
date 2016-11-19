//
//  paxosInstance.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include <algorithm>
#include "paxosInstance.h"
#include "jsonPaxos.h"
#include "UDPClient.h"

namespace Paxos
{
    Instance* Instance::_paxos_instance = nullptr;
    
    bool Instance::Create()
    {
        if( _paxos_instance == nullptr )
        {
            
            _paxos_instance = new Instance(new GlobalLog("Paxos", LL_DEBUG));
            if( _paxos_instance == nullptr )
                return false;
        }
        return true;
    }
    void Instance::Destory()
    {
        if(_paxos_instance)
        {
            delete _paxos_instance;
            _paxos_instance = nullptr;
        }
    }
    Instance* Instance::GetInstance()
    {
        return _paxos_instance;
    }
    
    
    
    Instance::Instance(ILog* ptrLog) : loop(this, ptrLog), proposal(this, ptrLog),
            acceptor(this, ptrLog), learner(this, ptrLog), logger(ptrLog)
    {
        
    }
    
    Instance::~Instance()
    {
        delete logger;
    }
    
    void Instance::PushToMessageQueue(IPacket* p)
    {
        // add message to message queue, message queue will notify the instance again
        loop.AddMessage(p);
    }
    
    void Instance::ProcessMessage(IPacket* p)
    {
        
    }
    
    void Instance::UpdateServerList(ADCS::ServerList list)
    {
        this->m_aliveSrvList = list;
    }
    
    void Instance::OnTimeout(unsigned int id, TimeoutType type)
    {
        switch(type)
        {
            case TimeoutType::Proposal_Accept:
                proposal.OnAcceptTimeout();
                break;
            case TimeoutType::Proposal_Prepare:
                proposal.OnPrepareTimeout();
                break;
                
            default:
                break;
        }
    }
    
    bool Instance::SendMessage(int nNodeID, IPacket* paxosPackage)
    {
        // find server by nodeid
        auto server = std::find_if(m_aliveSrvList.begin(), m_aliveSrvList.end(),
                     [nNodeID](const ADCS::ServerDesc &s) -> bool{
                                    return s.nodeid == nNodeID;
                                });
        
        if( server != m_aliveSrvList.end() )
        {
            return __send__udp_message__(server->address.c_str(), server->port, paxosPackage);
        }
        else
        {
            logger->Warning("Paxos::Instance::SendMessage failed, node[id:%d] not exist", nNodeID);
        }

        return true;
    }
    
    bool Instance::BroadcastMessage(IPacket* paxosPackage)
    {
        ADCS::ServerList srvList = m_aliveSrvList;
        for( size_t i = 0; i < srvList.size(); i++ )
        {
            __send__udp_message__(srvList[i].address.c_str(), srvList[i].port, paxosPackage);
        }
        return true;
    }
    
    bool Instance::__send__udp_message__(const char* szServerIP, int port, IPacket* paxosPackage)
    {
        CUDPClient client;
        client.SetIP(szServerIP);
        client.SetPort(port);
        
        if( client.Connect() )
        {
            const char* ptrResponse = nullptr;
            unsigned long ulResponseLength = 0;
            if( paxosPackage) paxosPackage->ToBytes(ptrResponse, ulResponseLength);
            
            int nSentLen = client.SendInfo(ptrResponse, (int)ulResponseLength);
            if( nSentLen != (int)ulResponseLength)
            {
                logger->Error("Paxos::Instance, send message error, require to send %d, in fact it sent %d", ulResponseLength, nSentLen);
                return false;
            }
        }
        return true;
    }
    
}
