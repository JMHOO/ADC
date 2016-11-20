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
#include "paxosStorage.h"

namespace Paxos
{
    Acceptor::Acceptor(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog), m_storage(ptrLog)
    {
        m_storage.Init();
    }
    
    bool Acceptor::Initialize()
    {
        uint64_t lMaxInstanceID = m_storage.GetMaxInstanceID();
        if( lMaxInstanceID == -1 )
        {// no records exist in file
            lMaxInstanceID = 0;
        }
        
        return true;
    }
    void Acceptor::NewTransaction()
    {
        
    }
    
    void Acceptor::ProcessMessage(IPacket* p)
    {
        
    }
}
