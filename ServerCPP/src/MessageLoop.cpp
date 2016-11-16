//
//  MessageLoop.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/14/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "MessageLoop.h"
#include "PackageInterface.h"
#include "paxosInstance.h"
#include "Utility.h"

MessageLoop::MessageLoop(Paxos::Instance * ptrInstance) : m_pInstance(ptrInstance)
{
    m_bExitFlag = false;
    m_bIsStart = false;
    
    m_currentTimerID = 1;
}

MessageLoop::~MessageLoop()
{
}

void MessageLoop::Run()
{
    m_bExitFlag = false;
    m_bIsStart = true;
    while(true)
    {
        int nNextTimeout = 1000;
        
        CheckTimeOut(nNextTimeout);
        
        Process(nNextTimeout);
        
        if(m_bExitFlag)
        {
            break;
        }
    }
}

void MessageLoop::AddNotify()
{
    m_messageQueue.lock();
    m_messageQueue.add(nullptr);
    m_messageQueue.unlock();
}

int MessageLoop::AddMessage(IPacket* packet)
{
    m_messageQueue.lock();
    
    if ((int)m_messageQueue.size() > MAX_QUEUE_LENGTH)
    {
        //logger->Error("too many message in the waitting list, skip msg");
        m_messageQueue.unlock();
        return -2;
    }
    
    m_messageQueue.add(new IPacket(*packet));
    m_messageQueue.unlock();
    
    return 0;
}

void MessageLoop::Stop()
{
    m_bExitFlag = true;
    if (m_bIsStart)
    {
        this->Join();
    }
}

void MessageLoop::Process(const int nWaitTimeoutMS)
{
    IPacket* p = nullptr;
    
    m_messageQueue.lock();
    bool bSucc = m_messageQueue.front(p, nWaitTimeoutMS);
    
    if (!bSucc)
    {
        m_messageQueue.unlock();
    }
    else
    {
        m_messageQueue.pop();
        m_messageQueue.unlock();
        
        if (p != nullptr )
        {
            m_pInstance->ProcessMessage(p);
        }
        
        delete p;
    }
    
    //m_Instance->CheckNewValue();
}

bool MessageLoop::AddTimer(const int nTimeoutMS, const TimeoutType type, unsigned int& id)
{
    if (nTimeoutMS <= 0){ return true; }
    
    uint64_t lTimeoutMoment = ADCS::Utility::GetCurrentTimeMS() + nTimeoutMS;
    
    id = m_currentTimerID++;
    m_timers.push(Timer(id, lTimeoutMoment, type));
    m_timerMarker[id] = true;
    
    return true;
}

void MessageLoop::RemoveTimer(unsigned int& id)
{
    auto it = m_timerMarker.find(id);
    if (it != m_timerMarker.end())
    {
        m_timerMarker.erase(it);
    }
    
    id = 0;
}

void MessageLoop::ProcessTimeout(const unsigned int id, const TimeoutType type)
{
    auto it = m_timerMarker.find(id);
    if (it == m_timerMarker.end())
    {
        // timer already removed, ignore it
        return;
    }
    
    m_timerMarker.erase(it);
    
    
    m_pInstance->OnTimeout(id, type);
}

void MessageLoop::CheckTimeOut(int& nNextTimeout)
{
    bool bTimeout = true;
    
    while(bTimeout)
    {
        unsigned int id = 0;
        TimeoutType type = TimeoutType::None;
        
        bTimeout = PopTimer(id, type);
        
        if (bTimeout)
        {
            ProcessTimeout(id, type);
            
            // still have time, end this round
            if (GetNextTimeout() != 0)
            {
                break;
            }
        }
    }
}


int MessageLoop::GetNextTimeout()
{
    if (m_timers.empty())
    {
        return -1;
    }
    
    int nSpanOfNextTimeout = 0;
    
    Timer t = m_timers.top();
    uint64_t now = ADCS::Utility::GetCurrentTimeMS();
    if (t.m_time > now)
    {
        nSpanOfNextTimeout = (int)(t.m_time - now);
    }
    
    return nSpanOfNextTimeout;
}

bool MessageLoop::PopTimer(unsigned int& id, TimeoutType& type)
{
    if (m_timers.empty())
    {
        return false;
    }
    
    Timer t = m_timers.top();
    uint64_t now = ADCS::Utility::GetCurrentTimeMS();
    if (t.m_time > now)
    {
        return false;
    }
    
    m_timers.pop();
    
    id = t.m_id;
    type = t.m_type;
    
    return true;
}

