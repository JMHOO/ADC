//
//  paxosStorage.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosStorage_H_
#define _ADCS__paxosStorage_H_

class ILog;

namespace Paxos
{
    class Storage
    {
    public:
        Storage(ILog* ptrLog);
        
    private:
        ILog* logger;
    };
}

#endif /* paxosStorage_h */
