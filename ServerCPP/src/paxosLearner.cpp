//
//  paxosLearner.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosLearner.h"

namespace Paxos
{
    Learner::Learner(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog)
    {
        
    }
}
