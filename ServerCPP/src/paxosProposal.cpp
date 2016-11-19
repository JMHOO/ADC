//
//  paxosProposal.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosInstance.h"
#include "jsonPaxos.h"
#include "MessageLoop.h"
#include "paxosProposal.h"
#include "GLog.h"

namespace Paxos
{
    Proposal::Proposal(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog), counter(instance)
    {
        m_proposalID = 1;
        NewTransaction();
        
        m_idPrepareTimer = 0;
        m_idAcceptTimer = 0;
        
        m_state = State::Idle;
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
    
    void Proposal::Prepare(bool bUseNewID)
    {
        logger->Info("Proposal::Prepare node id:%ld, instance id:%lu, proposal id:%lu", m_pInstance->GetInstanceID(),
                     m_pInstance->GetNodeID(), m_proposalID);
        
        ExitAccept();
        m_state = State::Preparing;

        
        m_otherPreAcceptedID.reset();
        if( bUseNewID)
        {
            NewPrepare();
        }
        
        jsonPaxos p;
        p.SetMessageType(PaxosType::Prepare);
        p.SetInstanceID(m_pInstance->GetInstanceID());
        p.SetNodeID(m_pInstance->GetNodeID());
        p.SetProposalID(m_proposalID);
        
        counter.NewRound();
        
        AddPrepareTimeout();
        
        m_pInstance->BroadcastMessage(&p);
    }
    
    void Proposal::Accept()
    {
        
    }
    
    
    void Proposal::AddPrepareTimeout(const int nTimeout)
    {
     
        MessageLoop* loop = m_pInstance->GetMessageLoop();
        if (m_idPrepareTimer > 0)
        {
            loop->RemoveTimer(m_idPrepareTimer);
        }
        
        if (nTimeout > 0)
        {
            loop->AddTimer(nTimeout, TimeoutType::Proposal_Prepare, m_idPrepareTimer);
        }
        else
        {
            loop->AddTimer(C_DEFAULT_TIMEOUT, TimeoutType::Proposal_Prepare, m_idPrepareTimer);
            //m_llTimeoutInstanceID = GetInstanceID();
            //m_iLastPrepareTimeoutMs *= 2;
            //if (m_iLastPrepareTimeoutMs > MAX_PREPARE_TIMEOUTMS)
            //{
            //    m_iLastPrepareTimeoutMs = MAX_PREPARE_TIMEOUTMS;
            //}
        }
        //logger->Info("Proposal::AddPrepareTimeout %d ms", )
    }
    
    void Proposal::OnPrepareTimeout()
    {
        
    }
    
    void Proposal::OnAcceptTimeout()
    {
        
    }
    
    void Proposal::ExitPrepare()
    {
        m_state = Idle;
        m_pInstance->GetMessageLoop()->RemoveTimer(m_idPrepareTimer);
    }
    
    void Proposal::ExitAccept()
    {
        m_state = Idle;
        m_pInstance->GetMessageLoop()->RemoveTimer(m_idAcceptTimer);
    }
}
