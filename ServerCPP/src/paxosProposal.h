//
//  paxosProposal.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosProposal_H_
#define _ADCS__paxosProposal_H_

class ILog;

namespace Paxos
{
    class Proposal
    {
    public:
        Proposal(ILog* ptrLog);
        ~Proposal();
        
        
    public:
        
        void OnPrepareTimeout();
        void OnAcceptTimeout();

    private:
        ILog* logger;
    };
}


#endif /* paxosProposal_h */
