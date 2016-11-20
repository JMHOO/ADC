//
//  paxosLearner.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include <string>
#include "paxosLearner.h"
#include "jsonPaxos.h"
#include "paxosInstance.h"
#include "paxosAcceptor.h"
#include "paxosStorage.h"
#include "GLog.h"
#include "jsonkvPackage.h"

using namespace std;

namespace Paxos
{
	Learner::Learner(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog)
	{
		m_pInstance = (Instance *)instance;
	}
    
	void Learner::NewTransaction()
	{
        m_bIsLearned = false;
        m_learnedValue = "";
	}

    bool Learner::IsLearned() const
    {
        return m_bIsLearned;
    }
    
	void Learner::ProcessMessage(IPacket* p)
	{
		jsonPaxos* pm = dynamic_cast<jsonPaxos*>(p);
		PaxosType type = pm->GetMessageType();
		if( type == PaxosType::ChosenValue)
		{
			OnChosenValue(pm);
		}
	}

    void Learner::ProposalChosenValue(const uint64_t lInstanceID, const uint64_t lProposalID)
    {
        jsonPaxos p;
        p.SetMessageType(PaxosType::ChosenValue);
        p.SetInstanceID(lInstanceID);
        p.SetNodeID(m_pInstance->GetNodeID());
        p.SetProposalID(lProposalID);
        
        m_pInstance->BroadcastMessage(&p);
    }
    
	void Learner::OnChosenValue(jsonPaxos* p)
	{
        Acceptor& acceptor = m_pInstance->GetAcceptor();
        logger->Info("Learner::OnChosenValue, now instance id:%lu, Message[instance id:%lu, proposal id:%lu, from node:%d] accepted proposal id:%lu, accepted node id:%d",
                     m_pInstance->GetInstanceID(), p->GetInstanceID(), p->GetProposalID(), p->GetNodeID(), acceptor.GetAcceptedID().ProposalID, acceptor.GetAcceptedID().NodeID);
        
        if (p->GetInstanceID() != m_pInstance->GetInstanceID() )
        {
            logger->Info("Learner::OnChosenValue, instance id not same, ignore message");
            return;
        }
        
        if( !acceptor.GetAcceptedID().isValid())
        {
            logger->Info("Learner::OnChosenValue, have not accepted any proposal, ignore message");
            return;
        }
        
        IDNumber idFrom(p->GetProposalID(), p->GetNodeID());
        if( idFrom != acceptor.GetAcceptedID() )
        {
            logger->Info("Learner::OnChosenValue, accepted ID not same as this one, ignore message");
            return;
        }
        
        
        //learn value
        LearnValue(p->GetInstanceID(), idFrom, acceptor.GetAcceptedValue());
    }
    
    void Learner::LearnValue(uint64_t lInstanceID, IDNumber& learnedID, const std::string& learnedValue)
    {
        Storage s(logger);
        s.Init();
        
        s.write(lInstanceID, learnedID.ProposalID, learnedValue);
        
        m_learnedValue = learnedValue;
        m_bIsLearned = true;
        
        
        // set value to kvserver
        m_pInstance->OnCommitComplete(m_learnedValue);
    }

}
