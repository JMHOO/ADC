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
        
        AddTimeout(TimeoutType::Proposal_Prepare);
        
        m_pInstance->BroadcastMessage(&p);
    }
    
    void Proposal::Accept()
    {
        logger->Info("Proposal::Accept proposal id:%lu, value:%s", m_proposalID, m_value.c_str());
        
        ExitPrepare();
        m_state = State::Accepting;
        
        jsonPaxos p;
        p.SetMessageType(PaxosType::Accept);
        p.SetInstanceID(m_pInstance->GetInstanceID());
        p.SetNodeID(m_pInstance->GetNodeID());
        p.SetProposalID(m_proposalID);
        p.SetValue(m_value);
        
        counter.NewRound();
        
        AddTimeout(TimeoutType::Proposal_Accept);
        
        m_pInstance->BroadcastMessage(&p);
    }
    
    
    void Proposal::AddTimeout(TimeoutType tt, const int nTimeout)
    {
     
        MessageLoop* loop = m_pInstance->GetMessageLoop();
        
        if (m_idPrepareTimer > 0 && tt == TimeoutType::Proposal_Prepare)
        {
            loop->RemoveTimer(m_idPrepareTimer);
        }
        
        int nNewTimeout = nTimeout;
        if( nTimeout == 0 )
        {
            nNewTimeout = C_DEFAULT_TIMEOUT;
        }
        
        
        loop->AddTimer(nNewTimeout, tt, tt == TimeoutType::Proposal_Prepare ? m_idPrepareTimer : m_idAcceptTimer);
       
            //m_llTimeoutInstanceID = GetInstanceID();
            //m_iLastPrepareTimeoutMs *= 2;
            //if (m_iLastPrepareTimeoutMs > MAX_PREPARE_TIMEOUTMS)
            //{
            //    m_iLastPrepareTimeoutMs = MAX_PREPARE_TIMEOUTMS;
            //}
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
