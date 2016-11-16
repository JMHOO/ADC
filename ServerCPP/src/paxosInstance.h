//
//  paxosInstance.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosInstance_H_
#define _ADCS__paxosInstance_H_

#include "MessageLoop.h"

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
        Instance();
        
        void ProcessPackage(IPacket* p);
        
        
    private:
        MessageLoop loop;
        
        static Instance*   _paxos_instance;
    };
}


#endif /* paxosInstance_h */
