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
    Proposal::Proposal(ILog* ptrLog) : logger(ptrLog)
    {
        
    }
    
    Proposal::~Proposal()
    {
        
    }
    
    void Proposal::OnPrepareTimeout()
    {
        
    }
    
    void Proposal::OnAcceptTimeout()
    {
        
    }
}
