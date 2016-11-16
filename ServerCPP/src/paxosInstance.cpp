//
//  paxosInstance.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosInstance.h"
#include "jsonPaxos.h"

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
    
    
    
    Instance::Instance(ILog* ptrLog) : loop(this), proposal(ptrLog), acceptor(ptrLog), learner(ptrLog), logger(ptrLog)
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
    
}
