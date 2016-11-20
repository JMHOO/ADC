//
//  paxosAcceptor.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosAcceptor_H_
#define _ADCS__paxosAcceptor_H_

#include "paxosFoundation.h"
#include "paxosStorage.h"
#include <string>

class ILog;
class IPacket;

namespace Paxos
{
    class Instance;
    class Acceptor
    {
    public:
        Acceptor(Paxos::Instance * instance, ILog* ptrLog);
        bool Initialize();
        
        void ProcessMessage(IPacket* p);
        
        void NewTransaction();
        
        IDNumber& GetAcceptedID();
        std::string GetAcceptedValue() const;

    private:
        Paxos::Instance * m_pInstance;
        ILog* logger;

        IDNumber m_promisedID;
        IDNumber m_acceptedID;
        std::string m_acceptedValue;
        Storage m_storage;
        
        bool WriteStates();
        void OnRecvPrepare(IPacket* p);
        void OnRecvAccept(IPacket* p);
    };
}


#endif /* paxosAcceptor_h */
