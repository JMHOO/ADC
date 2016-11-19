//
//  paxosFoundation.h
//  ServerCPP
//
//  Created by Jiaming on 11/19/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosFoundation_H_
#define _ADCS__paxosFoundation_H_
#include <inttypes.h>

namespace Paxos
{
    const int C_DEFAULT_TIMEOUT = 3000;
    
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

}
#endif /* paxosFoundation_h */
