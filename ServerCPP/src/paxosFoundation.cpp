//
//  paxosFoundation.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/19/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosFoundation.h"

namespace Paxos
{
    
    IDNumber::IDNumber() : ProposalID(0), NodeID(0)
    { }
    
    IDNumber::IDNumber(uint64_t proposalID, int nodeID) : ProposalID(proposalID), NodeID(nodeID)
    { }
    
    IDNumber::~IDNumber() { }
    
    bool IDNumber::operator >= (const IDNumber & other) const
    {
        return ProposalID == other.ProposalID ? (NodeID >= other.NodeID) : (ProposalID >= other.NodeID);
    }
    
    bool IDNumber::operator > (const IDNumber & other) const
    {
        return ProposalID == other.ProposalID ? (NodeID > other.NodeID) : (ProposalID > other.NodeID);
    }
    
    bool IDNumber::operator != (const IDNumber & other) const
    {
        return ProposalID != other.ProposalID || NodeID != other.NodeID;
    }
    
    bool IDNumber::operator == (const IDNumber & other) const
    {
        return ProposalID == other.ProposalID && NodeID == other.NodeID;
    }
    
    const bool IDNumber::isValid() const
    {
        return ProposalID > 0;
    }
    
    void IDNumber::reset()
    {
        ProposalID = 0;
        NodeID = 0;
    }
}
