//
//  paxosLearner.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//


#ifndef _ADCS__paxosLearner_H_
#define _ADCS__paxosLearner_H_

#include <string>
#include "paxosFoundation.h"

class ILog;
class IPacket;
class jsonPaxos;

namespace Paxos
{
    class Instance;
    class Learner
    {
    public:
        Learner(Paxos::Instance * instance, ILog* ptrLog);
        
        void ProcessMessage(IPacket* p);
        
        void NewTransaction();
        
        void ProposalChosenValue(const uint64_t lInstanceID, const uint64_t lProposalID);
        void OnChosenValue(jsonPaxos* p);
        
    private:
        Paxos::Instance * m_pInstance;
        ILog* logger;
        
        bool m_bIsLearned;
        std::string m_learnedValue;
        
        void LearnValue(uint64_t lInstanceID, IDNumber& learnedID, const std::string& learnedValue);

    };
}
#endif /* paxosLearner_h */
