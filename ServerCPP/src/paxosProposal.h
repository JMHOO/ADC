//
//  paxosProposal.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosProposal_H_
#define _ADCS__paxosProposal_H_

#include "paxosFoundation.h"
#include "paxosCounter.h"
#include <string>

class ILog;

namespace Paxos
{
    class Instance;
    class Proposal
    {
    public:
        enum State { Idle = 0, Preparing, Accepting };
        
    public:
        Proposal(Paxos::Instance * instance, ILog* ptrLog);
        ~Proposal();
        
        void Prepare(bool bUseNewID = false);
        void Accept();
        void AddTimeout(TimeoutType tt, const int nTimeout = 0);
        
    public:
        void NewTransaction();
        void NewPrepare();
        void OnPrepareTimeout();
        void OnAcceptTimeout();

    private:
        Paxos::Instance * m_pInstance;
        ILog* logger;
        
        Counter counter;
        
        uint64_t m_proposalID;
        uint64_t m_otherHighestID;
        
        unsigned int m_idPrepareTimer;
        unsigned int m_idAcceptTimer;
        
        std::string m_value;
        
        State m_state;
        
        IDNumber m_otherPreAcceptedID;
        
        void ExitPrepare();
        void ExitAccept();
    };
}


#endif /* paxosProposal_h */
