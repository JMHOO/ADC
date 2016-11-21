//
//  MutexLock.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/19/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "MutexLock.h"


CMutexLock :: CMutexLock()// : m_Lock(m_Mutex)
{
    //m_Lock.unlock();
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
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Cond.wait(lock);
}

void CMutexLock::Interupt()
{
    m_Cond.notify_one();
}

bool CMutexLock :: WaitTime(const int iTimeMs)
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Cond.wait_for(lock, std::chrono::milliseconds(iTimeMs)) != std::cv_status::timeout;
}
