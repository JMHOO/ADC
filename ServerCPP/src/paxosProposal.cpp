//
//  paxosProposal.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosProposal.h"
#include "GLog.h"

namespace Paxos
{
    Proposal::Proposal(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog), counter(instance)
    {
        
    }
    
    Proposal::~Proposal()
    {
        
    }
    
    void Proposal::NewTransaction()
    {
        
    }
    
    void Proposal::OnPrepareTimeout()
    {
        
    }
    
    void Proposal::OnAcceptTimeout()
    {
        
    }
}
