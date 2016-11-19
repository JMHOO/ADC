//
//  paxosLearner.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//


#ifndef _ADCS__paxosLearner_H_
#define _ADCS__paxosLearner_H_

class ILog;

namespace Paxos
{
    class Instance;
    class Learner
    {
    public:
        Learner(Paxos::Instance * instance, ILog* ptrLog);
        
        void NewTransaction();
        
    private:
        Paxos::Instance * m_pInstance;
        ILog* logger;

    };
}
#endif /* paxosLearner_h */
