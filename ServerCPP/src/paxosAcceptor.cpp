//
//  paxosAcceptor.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosAcceptor.h"
#include "GLog.h"
#include "jsonPaxos.h"

namespace Paxos
{
    Acceptor::Acceptor(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog)
    {
        
    }
    
    void Acceptor::NewTransaction()
    {
        
    }
    
    void Acceptor::ProcessMessage(IPacket* p)
    {
        
    }
}
