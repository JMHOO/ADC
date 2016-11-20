//
//  MutexLock.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/19/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "MutexLock.h"


CMutexLock :: CMutexLock() : m_Lock(m_Mutex)
{
    m_Lock.unlock();
}

CMutexLock::~CMutexLock()
{
    
}

void CMutexLock::Lock()
{
    m_Mutex.lock();
}

void CMutexLock::UnLock()
{
    m_Mutex.unlock();
}

void CMutexLock::Wait()
{
    m_Cond.wait(m_Lock);
}

void CMutexLock::Interupt()
{
    m_Cond.notify_one();
}

bool CMutexLock :: WaitTime(const int iTimeMs)
{
    return m_Cond.wait_for(m_Lock, std::chrono::milliseconds(iTimeMs)) != std::cv_status::timeout;
}
