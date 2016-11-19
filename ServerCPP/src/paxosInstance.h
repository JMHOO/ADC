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
    
    struct IDNumber
    {
    public:
        IDNumber();
        IDNumber(uint64_t proposalID, int nodeID);
        ~IDNumber();
        
        bool operator >= (const IDNumber & other) const;
        bool operator != (const IDNumber & other) const;
        bool operator == (const IDNumber & other) const;
        bool operator > (const IDNumber & other) const;
        
        const bool isValid() const;
        void reset();
        
        uint64_t m_proposalID;
        int m_nodeID;
    };
    
    class Instance
    {
    public:
        static bool        Create();
        static void        Destory();
        static Instance*   GetInstance();
        
        
    public:
        Instance(ILog* ptrLog);
        ~Instance();
        
        
        uint64_t    GetInstanceID();
        void        SetInstanceID(const uint64_t id);
        void        NewTransaction();
        
        int NodeCount();
        int QuantumCount();
        void ProcessMessage(IPacket* p);
        void PushToMessageQueue(IPacket* p);
        
        void OnTimeout(unsigned int id, TimeoutType type);
        
        void UpdateServerList(ADCS::ServerList list);
        
        bool SendMessage(int nNodeID, IPacket* paxosPackage);
        bool BroadcastMessage(IPacket* paxosPackage);
        
    private:
        MessageLoop loop;
        
        Proposal proposal;
        Acceptor acceptor;
        Learner learner;
        
        ILog*  logger;
        
        ADCS::ServerList m_aliveSrvList;
        
        static Instance*   _paxos_instance;
        
        uint64_t m_ID64;
        
        bool __send__udp_message__(const char* szServerIP, int port, IPacket* paxosPackage);
    };
}


#endif /* paxosInstance_h */
