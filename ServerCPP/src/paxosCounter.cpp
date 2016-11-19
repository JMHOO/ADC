//
//  paxosCounter.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/16/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosCounter.h"
#include "paxosInstance.h"

namespace Paxos
{
    Counter::Counter(Paxos::Instance *instance) : m_pInstance(instance)
    {
        NewRound();
    }
    
    void Counter::NewRound()
    {
        for(int i = 0; i < Counter::Kinds::Count; i++ )
            Nodes[i].clear();
    }
    
    void Counter::Add(Counter::Kinds kind, const int nodeid)
    {
        auto c = std::find(Nodes[kind].begin(), Nodes[kind].end(), nodeid);
        if( c == Nodes[kind].end())
        {
            Nodes[kind].push_back(nodeid);
        }
    }
    
    bool Counter::IsAllReceived()
    {
        return (int)Nodes[Kinds::Received].size() == m_pInstance->NodeCount();
    }
    
    bool Counter::IsPassed()
    {
        return (int)Nodes[Kinds::Promised].size() >= m_pInstance->QuantumCount();
    }
    
    bool Counter::IsRejected()
    {
        return (int)Nodes[Kinds::Rejected].size() >= m_pInstance->QuantumCount();
    }
}
