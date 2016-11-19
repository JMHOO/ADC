//
//  paxosLearner.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosLearner.h"
#include "jsonPaxos.h"
#include "GLog.h"

namespace Paxos
{
    Learner::Learner(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog)
    {
        
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
}
