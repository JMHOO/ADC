//
//  paxosProposal.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosProposal_H_
#define _ADCS__paxosProposal_H_

#include "paxosCounter.h"

class ILog;

namespace Paxos
{
    class Instance;
    class Proposal
    {
    public:
        Proposal(Paxos::Instance * instance, ILog* ptrLog);
        ~Proposal();
        
        
    public:
        void NewTransaction();
        void OnPrepareTimeout();
        void OnAcceptTimeout();

    private:
        Paxos::Instance * m_pInstance;
        ILog* logger;
        
        Counter counter;
    };
}


#endif /* paxosProposal_h */
