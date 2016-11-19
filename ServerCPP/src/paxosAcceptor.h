//
//  paxosAcceptor.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosAcceptor_H_
#define _ADCS__paxosAcceptor_H_

class ILog;
class IPacket;

namespace Paxos
{
    class Instance;
    class Acceptor
    {
    public:
        Acceptor(Paxos::Instance * instance, ILog* ptrLog);

        void ProcessMessage(IPacket* p);
        
        void NewTransaction();

    private:
        Paxos::Instance * m_pInstance;
        ILog* logger;

    };
}


#endif /* paxosAcceptor_h */
