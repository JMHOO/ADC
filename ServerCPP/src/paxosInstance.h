//
//  paxosInstance.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosInstance_H_
#define _ADCS__paxosInstance_H_

#include "GLog.h"
#include "Network.h"
#include "MessageLoop.h"
#include "paxosProposal.h"
#include "paxosAcceptor.h"
#include "paxosLearner.h"


#define PaxosInstance Paxos::Instance::GetInstance()

class IPacket;

namespace Paxos
{
    class Instance
    {
    public:
        static bool        Create();
        static void        Destory();
        static Instance*   GetInstance();
        
        
    public:
        Instance(ILog* ptrLog);
        ~Instance();
        
        void ProcessMessage(IPacket* p);
        void PushToMessageQueue(IPacket* p);
        
        void OnTimeout(unsigned int id, TimeoutType type);
        
        void UpdateServerList(ADCS::ServerList list);
        
        
        
    private:
        MessageLoop loop;
        
        Proposal proposal;
        Acceptor acceptor;
        Learner learner;
        
        ILog*  logger;
        
        ADCS::ServerList m_aliveSrvList;
        
        static Instance*   _paxos_instance;
    };
}


#endif /* paxosInstance_h */
