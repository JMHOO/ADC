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
    class Learner
    {
    public:
        Learner(ILog* ptrLog);
        
    private:
        ILog* logger;

    };
}
#endif /* paxosLearner_h */
