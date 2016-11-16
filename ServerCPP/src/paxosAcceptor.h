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

namespace Paxos
{
    class Acceptor
    {
    public:
        Acceptor(ILog* ptrLog);


    private:
        ILog* logger;

    };
}


#endif /* paxosAcceptor_h */
