//
//  paxosStorage.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosStorage.h"
#include "GLog.h"

namespace Paxos
{
    Storage::Storage(ILog* ptrLog) : logger(ptrLog)
    {
        
    }
}
