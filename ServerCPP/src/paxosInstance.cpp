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
            _paxos_instance = new Instance();
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
    
    
    
    Instance::Instance() : loop(this)
    {
        
    }
    
    void Instance::ProcessPackage(IPacket* p)
    {
        loop.AddMessage(p);
    }
    
    
    
}
