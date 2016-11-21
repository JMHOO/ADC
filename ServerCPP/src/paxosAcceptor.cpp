//
//  paxosAcceptor.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosAcceptor.h"
#include "GLog.h"
#include "paxosInstance.h"
#include "jsonPaxos.h"
#include "paxosStorage.h"

namespace Paxos
{
    Acceptor::Acceptor(Paxos::Instance * instance, ILog* ptrLog) : m_pInstance(instance), logger(ptrLog), m_storage(ptrLog)
    {
        m_storage.Init();
    }
    
    bool Acceptor::Initialize()
    {
        uint64_t lInstanceID = m_storage.GetMaxInstanceID();
        if( lInstanceID == 0 )
        {// no records exist in file
            lInstanceID = 0;
            return true;
        }
        
        // read states from file
        m_storage.ReadState(m_promisedID.ProposalID, m_promisedID.NodeID, m_acceptedID.ProposalID, m_acceptedID.NodeID, m_acceptedValue);
        
        logger->Info("Acceptor initialized from file, instance id:%lu, promised node id:%d, promised proposal id:%lu, accepted node id:%d, accepted proposal id:%lu, value: %s",
                     lInstanceID, m_promisedID.NodeID, m_promisedID.ProposalID, m_acceptedID.NodeID, m_acceptedID.ProposalID, m_acceptedValue.c_str());
        
        m_pInstance->SetInstanceID(lInstanceID);
        
        return true;
    }
    void Acceptor::NewTransaction()
    {
        m_acceptedID.reset();
        m_acceptedValue = "";
        
    }
    
    IDNumber& Acceptor::GetAcceptedID()
    {
        return m_acceptedID;
    }
    
    std::string Acceptor::GetAcceptedValue() const
    {
        return m_acceptedValue;
    }
    
    void Acceptor::ProcessMessage(IPacket* p)
    {
        jsonPaxos* pm = (jsonPaxos*)p;
        PaxosType type = pm->GetMessageType();
        if( type == PaxosType::Prepare)
        {
            OnRecvPrepare(p);
        }
        else if( type == PaxosType::Accept)
        {
            OnRecvAccept(p);
        }
    }
    
    bool Acceptor::WriteStates()
    {
        return m_storage.WriteState(m_promisedID.ProposalID, m_promisedID.NodeID, m_acceptedID.ProposalID, m_acceptedID.NodeID, m_acceptedValue);
    }
    
    
    void Acceptor::OnRecvPrepare(IPacket* p)
    {
        jsonPaxos* pm = (jsonPaxos*)p;
        
        int nToNodeID = pm->GetNodeID();
        
        logger->Info("Acceptor::OnPrepare, instance id:%lu, from node:%d, proposal id:%lu",
                     pm->GetInstanceID(), pm->GetNodeID(), pm->GetProposalID());
        
        jsonPaxos response;
        response.SetMessageType(PaxosType::PrepareResponse);
        response.SetInstanceID(m_pInstance->GetInstanceID());
        response.SetNodeID(m_pInstance->GetNodeID());
        response.SetProposalID(pm->GetProposalID());
        
        IDNumber fromID(pm->GetProposalID(), pm->GetNodeID());
        
        if (fromID >= m_promisedID)
        {
            logger->Info("    Acceptor [Promise] current info[Promised node=%d, proposal id=%lu], [PreAccepted node=%d, proposal id=%lu]",
                         m_promisedID.NodeID, m_promisedID.ProposalID, m_acceptedID.NodeID, m_acceptedID.ProposalID);
            
            response.SetPreAcceptID(m_acceptedID.ProposalID);
            response.SetPreAcceptNodeID(m_acceptedID.NodeID);
            
            if (m_acceptedID.ProposalID > 0)
            {
                response.SetValue(m_acceptedValue);
            }
            
            m_promisedID = fromID;

            WriteStates();
        }
        else
        {
            logger->Info("    Acceptor [Reject] current info[Promised node=%d, proposal id=%lu",
                         m_promisedID.NodeID, m_promisedID.ProposalID);
            
            response.SetRejectPromiseID(m_promisedID.ProposalID);
        }
        
        m_pInstance->SendMessage(nToNodeID, &response);
    }
    
    void Acceptor::OnRecvAccept(IPacket* p)
    {
        jsonPaxos* pm = (jsonPaxos*)p;
        
        int nToNodeID = pm->GetNodeID();
        
        logger->Info("Acceptor::OnAccept, instance id:%lu, from node:%d, proposal id:%lu",
                     pm->GetInstanceID(), pm->GetNodeID(), pm->GetProposalID());
        
        
        jsonPaxos response;
        response.SetMessageType(PaxosType::AcceptResponse);
        response.SetInstanceID(m_pInstance->GetInstanceID());
        response.SetNodeID(m_pInstance->GetNodeID());
        response.SetProposalID(pm->GetProposalID());
        

        IDNumber fromID(pm->GetProposalID(), pm->GetNodeID());
        if (fromID >= m_promisedID)
        {
            logger->Info("    Acceptor [Promise] current info[Promised node=%d, proposal id=%lu], [PreAccepted node=%d, proposal id=%lu]",
                         m_promisedID.NodeID, m_promisedID.ProposalID, m_acceptedID.NodeID, m_acceptedID.ProposalID);

            m_promisedID = fromID;
            m_acceptedID = fromID;
            m_acceptedValue = pm->GetValue();
            
            logger->Info("        Accepted value: %s", m_acceptedValue.c_str());
            WriteStates();
        }
        else
        {
            logger->Info("    Acceptor [Reject] current info[Promised node=%d, proposal id=%lu]",
                         m_promisedID.NodeID, m_promisedID.ProposalID);
            
            response.SetRejectPromiseID(m_promisedID.ProposalID);
        }
        
        m_pInstance->SendMessage(nToNodeID, &response);
    }
}
