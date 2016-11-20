//
//  MessageLoop.h
//  ServerCPP
//
//  Created by Jiaming on 11/14/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef MessageLoop_h
#define MessageLoop_h

#include <condition_variable>
#include <mutex>
#include <queue>
#include <map>
#include "Thread.h"

using namespace std;

// thread safe queue
template <class T>
class TSQueue {
public:
    TSQueue() : m_lock(m_mutex)
    {
        m_size = 0;
        m_lock.unlock();
    }
    
    T& front()
    {
        while (empty()) { m_cond.wait(m_lock); }
        return m_queue.front();
    }
    
    size_t front(T& value)
    {
        while (empty()) { m_cond.wait(m_lock); }
        value = m_queue.front();
        return m_size;
    }
    
    bool front(T& t, int timeoutMS)
    {
        while (empty())
        {
            if (m_cond.wait_for(m_lock, std::chrono::milliseconds(timeoutMS)) == std::cv_status::timeout)
            {
                return false;
            }
        }
        t = m_queue.front();
        return true;
    }
    
    size_t pop(T* values, size_t n)
    {
        while (empty())
        {
            m_cond.wait(m_lock);
        }
        
        size_t i = 0;
        while (!m_queue.empty() && i < n)
        {
            values[i] = m_queue.front();
            m_queue.pop_front();
            m_size--;
            i++;
        }
        
        return i;
    }
    
    size_t pop()
    {
        m_queue.pop_front();
        return --m_size;
    }
    
    size_t add(const T& t, bool signal = true, bool back = true)
    {
        if (back)
        {
            m_queue.push_back(t);
        }
        else
        {
            m_queue.push_front(t);
        }
        
        if (signal)
        {
            m_cond.notify_one();
        }
        
        return ++m_size;
    }
    
    bool empty() const{ return m_queue.empty(); }
    size_t size() const { return m_queue.size(); }
    void clear() {m_queue.clear(); }
    void signal() { m_cond.notify_one(); }
    void broadcast() { m_cond.notify_all(); }
    void lock() { m_lock.lock(); }
    void unlock() { m_lock.unlock(); }
    
    void swap(TSQueue& q)
    {
        m_queue.swap( q.m_queue );
        int size = q.m_size;
        q.m_size = m_size;
        m_size = size;
    }
    
protected:
    std::mutex m_mutex;
    std::unique_lock<std::mutex> m_lock;
    std::condition_variable m_cond;
    deque<T> m_queue;
    size_t m_size;
};

enum class TimeoutType
{
    None,
    Proposal_Prepare,
    Proposal_Accept,
    Commit,
};


namespace Paxos
{
    class Instance;
}

class IPacket;
class ILog;

#define MAX_QUEUE_LENGTH 20

class MessageLoop : public ADCS::Thread
{
public:
    MessageLoop(Paxos::Instance * instance, ILog* ptrLog);
    virtual ~MessageLoop();
    
    virtual void Run();
    
    void Stop();
    void Process(const int nWaitTimeoutMS);
    
    void DoRetry();
    
    void ClearRetryQueue();
    
public:
    int AddMessage(IPacket* packet);
    void AddNotify();
    
public:
    bool AddTimer(const int nTimeoutMS, const TimeoutType type, unsigned int& id);
    void RemoveTimer(unsigned int & iTimerID);
    void CheckTimeOut(int& nNextTimeout);
    void ProcessTimeout(const unsigned int id, const TimeoutType type);
    
private:
    bool m_bExitFlag;
    bool m_bIsStart;
    
    unsigned int m_currentTimerID;
    std::map<unsigned int, bool> m_timerMarker;
    
    TSQueue<IPacket *> m_messageQueue;
    
    Paxos::Instance * m_pInstance;
    ILog* logger;
    
    int GetNextTimeout();
    bool PopTimer(unsigned int& id, TimeoutType& type);
    
private:
    struct Timer
    {
        Timer(unsigned int id, uint64_t time, TimeoutType type) : m_id(id), m_time(time), m_type(type) {}
        
        unsigned int m_id;
        uint64_t m_time;
        TimeoutType m_type;
        
        bool operator < (const Timer & obj) const
        {
            if (obj.m_time == m_time)
            {
                return obj.m_id < m_id;
            }
            else
            {
                return obj.m_time < m_time;
            }
        }
    };
    
    priority_queue<Timer, vector<Timer>, less<Timer> > m_timers;
};


#endif /* MessageLoop_h */
