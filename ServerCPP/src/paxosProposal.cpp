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
        m_proposalID = 1;
        NewTransaction();
    }
    
    Proposal::~Proposal()
    {
        
    }
    
    void Proposal::NewTransaction()
    {
        m_otherHighestID = 0;
        m_value = "";
    }
    
    void Proposal::NewPrepare()
    {
        logger->Info("Proposal new prepare, proposal id: %ld, other highest id: %ld", m_proposalID, m_otherHighestID);
        
        uint64_t maxProposalID = m_proposalID > m_otherHighestID ? m_proposalID : m_otherHighestID;
        
        m_proposalID = maxProposalID + 1;
    }
    
    void Proposal::OnPrepareTimeout()
    {
        
    }
    
    void Proposal::OnAcceptTimeout()
    {
        
    }
}
