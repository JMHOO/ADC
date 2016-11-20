//
//  MutexLock.h
//  ServerCPP
//
//  Created by Jiaming on 11/19/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef MutexLock_h
#define MutexLock_h

#include <mutex>
#include <condition_variable>


class CMutexLock
{
public:
    CMutexLock();
    ~CMutexLock();
    
    void Lock();
    void UnLock();
    
    void Wait();
    void Interupt();
    
    bool WaitTime(const int nTimeInMS);
    
private:
    std::mutex m_Mutex;
    std::unique_lock<std::mutex> m_Lock;
    std::condition_variable m_Cond;
};

#endif /* MutexLock_h */
