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
    
    IDNumber::IDNumber() : m_proposalID(0), m_nodeID(0)
    { }
    
    IDNumber::IDNumber(uint64_t proposalID, int nodeID) : m_proposalID(proposalID), m_nodeID(nodeID)
    { }
    
    IDNumber::~IDNumber() { }
    
    bool IDNumber::operator >= (const IDNumber & other) const
    {
        return m_proposalID == other.m_proposalID ? (m_nodeID >= other.m_nodeID) : (m_proposalID >= other.m_proposalID);
    }
    
    bool IDNumber::operator > (const IDNumber & other) const
    {
        return m_proposalID == other.m_proposalID ? (m_nodeID > other.m_nodeID) : (m_proposalID > other.m_proposalID);
    }
    
    bool IDNumber::operator != (const IDNumber & other) const
    {
        return m_proposalID != other.m_proposalID || m_nodeID != other.m_nodeID;
    }
    
    bool IDNumber::operator == (const IDNumber & other) const
    {
        return m_proposalID == other.m_proposalID && m_nodeID == other.m_nodeID;
    }
    
    const bool IDNumber::isValid() const
    {
        return m_proposalID > 0;
    }
    
    void IDNumber::reset()
    {
        m_proposalID = 0;
        m_nodeID = 0;
    }
}
