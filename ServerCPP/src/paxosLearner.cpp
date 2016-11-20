//
//  paxosLearner.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosLearner.h"
#include "jsonPaxos.h"
#include "paxosAcceptor.h"
#include "paxosCounter.h"
#include "paxosFoundation.h"
#include "paxosInstance.h"
#include "string.h"
#include "paxosInstance.h"
#include "jsonPaxos.h"
#include "MessageLoop.h"
#include "paxosProposal.h"
#include "GLog.h"
#include "jsonkvPackage.h"
#include<iostream>
using namespace std;
namespace Paxos
{
	Learner::Learner(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog)
	{
		m_pInstance = (Instance *)instance;
	}
	void Learner::NewTransaction()
	{

	}

	void Learner::ProcessMessage(IPacket* p)
	{
		jsonPaxos* pm = dynamic_cast<jsonPaxos*>(p);
		PaxosType type = pm->GetMessageType();
		if( type == PaxosType::ChosenValue)
		{
			OnChosenValue(p);
		}
	}

	void Learner::OnChosenValue(IPacket* p)
	{
		jsonPaxos* pm = dynamic_cast<jsonPaxos*>(p);

		std::string value = pm->GetValue();
	}
	int Learner :: SendLearnValue(
		const nodeid_t iSendNodeID,
		const uint64_t llLearnInstanceID,
		const IDNumber & oLearnedBallot,
		const std::string & sLearnedValue,
		const bool bNeedAck)
	{
		//
		jsonPaxos ojsonPaxos;
		ojsonPaxos.SetMessageType(PaxosType::ChosenValue);
		ojsonPaxos.SetNodeID(m_pInstance->GetNodeID);
		ojsonPaxos.SetValue(sLearnedValue);
		ojsonPaxos.SetProposalID(oLearnedBallot.m_proposalID);
		ojsonPaxos.SetInstanceID(llLearnInstanceID);


		return m_pInstance->SendMessage(iSendNodeID,&ojsonPaxos);

	}
	void Learner :: OnSendLearnValue(jsonPaxos & ojsonPaxos)
	{
		//learn value
		//IDNumber oBallot(ojsonPaxos.GetProposalID(), oPaxosMsg.proposalnodeid());

		jsonkvPacket jsonkvOperator(ojsonPaxos.GetValue());
		jsonkvOperator.Process();

	}
}