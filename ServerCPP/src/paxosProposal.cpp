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
#include <stdlib.h>

namespace Paxos
{
    Proposal::Proposal(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog), counter(instance)
    {
        m_proposalID = 1;
        NewTransaction();
        
        m_idPrepareTimer = 0;
        m_idAcceptTimer = 0;
        m_snapInstanceID_forTimeout = 0;
        
        m_state = State::Idle;
        
        m_bRejectedByOther = false;
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
    
    void Proposal::ProcessMessage(IPacket* p)
    {
        
    }
    
    void Proposal::Prepare(bool bUseNewID)
    {
        logger->Info("Proposal::Prepare node id:%ld, instance id:%lu, proposal id:%lu", m_pInstance->GetInstanceID(),
                     m_pInstance->GetNodeID(), m_proposalID);
        
        ExitAccept();
        m_state = State::Preparing;
        m_bRejectedByOther = false;
        
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
    
    
    void Proposal::OnPrepareResponse(IPacket* p)
    {
        jsonPaxos* pm = dynamic_cast<jsonPaxos*>(p);
        
        logger->Info("Proposal::OnPrepareResponse, my proposalid:%lu, receive proposalid:%lu, from node:%d, reject by id:%lu",
                     m_proposalID, pm->GetProposalID(), pm->GetNodeID(), pm->GetRejectPromiseID());
        
        if( m_state != State::Preparing )
        {
            // ignore, not preparing
            return;
        }
        
        if( pm->GetProposalID() != m_proposalID )
        {
            // ignore
            return;
        }
        
        counter.Add(Counter::Kinds::Received, pm->GetNodeID());
        
        // check if reject
        if( pm->GetRejectPromiseID() == 0 )
        {// no reject id, promised
            logger->Info("Proposal [Promised] PreAcceptID:%lu, PreAcceptNodeID:%d", pm->GetPreAcceptID(), pm->GetPreAcceptNodeID());
            
            counter.Add(Counter::Kinds::Promised, pm->GetNodeID());
            
            // add pre accept value
            IDNumber otherPreacceptID(pm->GetPreAcceptID(), pm->GetPreAcceptNodeID());
            if( otherPreacceptID.isValid() && otherPreacceptID > m_otherPreAcceptedID)
            {
                m_otherPreAcceptedID = otherPreacceptID;
                m_value = pm->GetValue();
            }
        }
        else
        {// reject by other
            logger->Info("Proposal [Reject] Reject by ID: %lu", pm->GetRejectPromiseID());
            counter.Add(Counter::Kinds::Rejected, pm->GetNodeID());
            m_bRejectedByOther = true;
            
            if (pm->GetRejectPromiseID() > m_otherHighestID)
            {
                m_otherHighestID = pm->GetRejectPromiseID();
            }
            
        }
        
        if( counter.IsPassed())
        {
            // prepare complete
            Accept();
        }
        else if( counter.IsRejected() )
        {
            // restart wait random time
            srand((unsigned int)time(NULL));
            int x = rand()%30 + 10;
            AddTimeout(TimeoutType::Proposal_Prepare, x);
        }
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
    
    
    
    void Proposal::OnAcceptResponse(IPacket * p)
    {
        jsonPaxos *pm = dynamic_cast<jsonPaxos*>(p);
        logger->Info("Proposal::OnAcceptResponse, myproposal id:%lu, proposal id:%lu, from node:%d, reject by ID: %lu",
                     m_proposalID, pm->GetProposalID(), pm->GetNodeID(), pm->GetRejectPromiseID());
        
        if( m_state != State::Accepting )
        {
            // ignore, not prosposaling
            return;
        }
        
        if( pm->GetProposalID() != m_proposalID )
        {
            // ignore, proposal id not same
        }
        
        
        counter.Add(Counter::Kinds::Received, pm->GetNodeID());
        
        if( pm->GetRejectPromiseID() == 0 )
        {
            logger->Info("Proposal::OnAcceptResponse [Accept]");
            counter.Add(Counter::Kinds::Promised, pm->GetNodeID());
        }
        else
        {
            logger->Info("Proposal::OnAcceptResponse [Reject]");
            counter.Add(Counter::Kinds::Rejected, pm->GetNodeID());
            
            // reject by someone
            m_bRejectedByOther = true;
            if (pm->GetRejectPromiseID() > m_otherHighestID)
            {
                m_otherHighestID = pm->GetRejectPromiseID();
            }
        }
        
        if( counter.IsPassed())
        {
            ExitAccept();
            // send value to learner
            //m_poLearner->ProposerSendSuccess(GetInstanceID(), m_oProposerState.GetProposalID());
        }
        else if( counter.IsRejected() )
        {
            srand((unsigned int)time(NULL));
            int x = rand()%30 + 10;
            AddTimeout(TimeoutType::Proposal_Accept, x);
        }
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
       
        m_snapInstanceID_forTimeout = m_pInstance->GetInstanceID();
        
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
        
        if (m_pInstance->GetInstanceID() != m_snapInstanceID_forTimeout)
        {
            logger->Info("Timeout instance id: %lu not same as now instance id: %lu, ignore.",
                         m_snapInstanceID_forTimeout, m_pInstance->GetInstanceID());
            return;
        }
        
        Prepare(m_bRejectedByOther);
    }
    
    void Proposal::OnAcceptTimeout()
    {
        if (m_pInstance->GetInstanceID() != m_snapInstanceID_forTimeout)
        {
            logger->Info("Timeout instance id: %lu not same as now instance id: %lu, ignore.",
                         m_snapInstanceID_forTimeout, m_pInstance->GetInstanceID());
            return;
        }
        
        Prepare(m_bRejectedByOther);
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
