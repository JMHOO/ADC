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
#include "MutexLock.h"
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
        
        bool Initialize();
        
        std::string ProposeNewValue(const std::string value);
        void CheckForNewProposeValue();
        
        uint64_t    GetInstanceID();
        void        SetInstanceID(const uint64_t id);
        void        NewTransaction();
        int         GetNodeID();
        Acceptor&   GetAcceptor();
        
        int NodeCount();
        int QuantumCount();
        void ProcessMessage(IPacket* p);
        void PushToMessageQueue(IPacket* p);
        
        void OnTimeout(unsigned int id, TimeoutType type);
        
        void UpdateServerList(ADCS::ServerList list, int nodeid);
        
        bool SendMessage(int nNodeID, IPacket* paxosPackage);
        bool BroadcastMessage(IPacket* paxosPackage);
        
        MessageLoop* GetMessageLoop(){ return &loop; }
        
        void ProposalChosenValue(const uint64_t lProposalID);
        
        void OnCommitComplete(std::string strValue);
        void OnCommitTimeout();
        
    private:
        MessageLoop loop;
        
        Proposal proposal;
        Acceptor acceptor;
        Learner  learner;
        ILog*    logger;
        
        ADCS::ServerList m_aliveSrvList;
        
        uint64_t    m_ID64;     // Instance ID
        int         m_nodeid;

        CMutexLock      m_sLocker;
        bool            m_bCommitting;
        unsigned int    m_idCommitTimer;
        std::string     m_strRequestValue;
        std::string     m_strResult;
        uint64_t        m_commitingInstanceID;
        
        static Instance* _paxos_instance;

        bool __send__udp_message__(const char* szServerIP, int port, IPacket* paxosPackage);
    };
}


#endif /* paxosInstance_h */
