//
//  paxosCounter.h
//  ServerCPP
//
//  Created by Jiaming on 11/16/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosCounter_H_
#define _ADCS__paxosCounter_H_

#include <vector>

class ILog;

typedef std::vector<int> NodeVector;

namespace Paxos
{
    class Instance;
    class Counter
    {
    public:
        enum Kinds{ Received = 0, Rejected = 1, Promised = 2, Count };
        
        Counter(Paxos::Instance * instance);
        
        void NewRound();
        void Add(Counter::Kinds kind, const int nodeid);
        
        bool IsAllReceived();
        bool IsPassed();
        bool IsRejected();
        
    private:
        Paxos::Instance * m_pInstance;
        NodeVector Nodes[Kinds::Count];
    };
}

#endif /* paxosCounter_h */
