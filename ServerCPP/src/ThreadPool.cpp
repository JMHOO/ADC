#include "ThreadPool.h"
#include <stdio.h>
#include <new>

namespace ADCS
{

bool CThreadPool::Init( IExecuteor &executor, unsigned int uiInitialCount, unsigned int uiCapacity, unsigned int uiMaxCount )
{
    SetExecutor( executor );
    return _initialize(uiInitialCount, uiCapacity, uiMaxCount);
}

bool CThreadPool::_initialize(  unsigned int uiInitialCount, unsigned int uiCapacity, unsigned int uiMaxCount)
{
    if( bInit )
        return true;
    
    m_BurstThreadCount = 0;
    m_WorkingThreadCount = 0;
    m_ThreadCount = 0;
    
    m_MaxThreadCount = uiMaxCount;
    m_PoolCapacity = uiCapacity;
    m_eachIncreaseCount = uiInitialCount;

    bPoolAlive = true;
    
    pthread_mutex_init( &Mutex, NULL );
    pthread_cond_init( &Condition, NULL );
    pthread_cond_init( &DetachCondition, NULL );
    
    
    if( uiCapacity > 0 && uiInitialCount > 0)
    {
        for( unsigned int i = 0; i < uiInitialCount; i++ )
        {
            Thread *ptrThread = new Thread;
            ptrThread->bBurst = false;
            ptrThread->pool = this;
            ptrThread->id = i;
            
            if( pthread_create( &ptrThread->pthread, NULL, _thread_run_, (void *)ptrThread ) == 0 )
            {
                m_threads.push_back(ptrThread);
                m_ThreadCount += 1;
            }
            else
            {
                delete ptrThread;
            }
        }
        
        if( m_ThreadCount == 0 )
        {
            return false;
        }
    }
    this->bInit = true;
    
    return true;
}

bool CThreadPool::WakeUp(void * lpvoid)
{
    if( !bInit )
        return false;
    
    Job* pNewJob = new Job();
    pNewJob->data = lpvoid;
    
    pthread_mutex_lock( &Mutex );
    {
        m_JobQueue.push(pNewJob);
        
        // check if we have enough idle thread for jobs.
        if( m_WorkingThreadCount + m_JobQueue.size() >= m_ThreadCount )
        {
            SupplementThreads();
        }
    }
    
    // notify any idle thread that have a job to do.
    pthread_cond_signal(&Condition);
    pthread_mutex_unlock( &Mutex );
    
    return true;
}


bool CThreadPool::SupplementThreads()
{
    if( m_eachIncreaseCount )
    {
        unsigned int uiAppendNum = 0;
        
        // thread count beyond pool's capacity, mark them as burst thread
        if( m_ThreadCount >= m_PoolCapacity )
        {
            if( m_MaxThreadCount > 0 && m_BurstThreadCount + m_ThreadCount >= m_MaxThreadCount)
            {
                return false;
            }
            return AppendThread( true );
        }
        else
            uiAppendNum = ((m_PoolCapacity - m_ThreadCount) >= m_eachIncreaseCount) ? m_eachIncreaseCount : (m_PoolCapacity - m_ThreadCount);
        
        for( unsigned int i = 0; i < uiAppendNum; i++ )
        {
            if( !AppendThread( false ) )
                return i > 0 ? true : false;
        }
    }
    
    return true;
}

//====================================================================
bool CThreadPool::AppendThread(bool bBurst)
{
    Thread* ptrThread = new Thread;
    ptrThread->pool = this;
    ptrThread->bBurst = bBurst;
    
    if( pthread_create( &ptrThread->pthread, NULL, _thread_run_, (void *)ptrThread ) == 0 )
    {
        if( bBurst )
        {
            ptrThread->id = m_BurstThreadCount;
            m_BurstThreadCount += 1;
        }
        else
        {
            ptrThread->id = m_ThreadCount;
            m_threads.push_back(ptrThread);
            m_ThreadCount += 1;
        }
    }
    else
    {
        delete ptrThread;
        return false;
    }
    
    return true;
}

void* CThreadPool::_thread_run_(void* lpParam)
{
    Thread* ptrThread = static_cast<Thread*>(lpParam);
    CThreadPool* pool = ptrThread->pool;
    
    while(1)
    {
        pthread_mutex_lock( &(pool->Mutex) );
        
        // no jobs anymore
        while( pool->m_JobQueue.size() == 0 )
        {
            // if this is burst thread, let it go
            if( ptrThread->bBurst )
            {
                pool->m_BurstThreadCount -= 1;
                
                delete ptrThread;
                
                // someone maybe wait for burst thread exit.
                pthread_cond_signal(&(pool->DetachCondition));
                pthread_mutex_unlock( &(pool->Mutex) );
                return NULL;
            }
            
            // The whole pool will be destoried
            if( !pool->bPoolAlive )
            {
                pool->m_ThreadCount -= 1;
                
                delete ptrThread;
                pthread_mutex_unlock( &(pool->Mutex) );
                return NULL;
            }
            
            // halting for new jobs.
            pthread_cond_wait( &(pool->Condition), &(pool->Mutex) );
        }
        
        // we have job to do
        Job* job = pool->m_JobQueue.front();
        pool->m_JobQueue.pop();
        
        // something wrong here
        if( job == NULL )
        {
            if( ptrThread->bBurst)
            {
                pool->m_BurstThreadCount -= 1;
                
                delete ptrThread;
                
                // someone maybe wait for burst thread exit.
                pthread_cond_signal(&(pool->DetachCondition));
                pthread_mutex_unlock( &(pool->Mutex) );
                return NULL;
            }
            else
            {
                pthread_mutex_unlock( &(pool->Mutex) );
                continue;
            }
        }

        if( !ptrThread->bBurst)
        {
            pool->m_WorkingThreadCount += 1;
        }
        pthread_mutex_unlock( &(pool->Mutex) );
        
        
        //---------- Running the job. -----------------------
        {
            if( pool->m_ptrExecutor )
                pool->m_ptrExecutor->Execute( job->data );
            
            delete job;
        }
        
        if( !ptrThread->bBurst)
        {
            pthread_mutex_lock( &(pool->Mutex) );
            pool->m_WorkingThreadCount -= 1;
            pthread_mutex_unlock( &(pool->Mutex) );
        }
    }
}

void CThreadPool::GetStatus( unsigned int &uiThreadCount, unsigned int &uiBurstThreadCount, unsigned int &uiWorkingThreadCount, unsigned int &uiJobCount )
{
    if( !bInit )
        return;
    
    pthread_mutex_lock( &Mutex );
    uiThreadCount    = m_ThreadCount;
    uiBurstThreadCount = m_BurstThreadCount;
    uiWorkingThreadCount      = m_WorkingThreadCount;
    uiJobCount  = (unsigned int)m_JobQueue.size();
    pthread_mutex_unlock( &Mutex );
}

void CThreadPool::GetStatusWithoutLock( unsigned int &uiThreadCount, unsigned int &uiBurstThreadCount, unsigned int &uiWorkingThreadCount, unsigned int &uiJobCount )
{
    uiThreadCount    = m_ThreadCount;
    uiBurstThreadCount = m_BurstThreadCount;
    uiWorkingThreadCount      = m_WorkingThreadCount;
    uiJobCount  = (unsigned int)m_JobQueue.size();
}

void CThreadPool::Destory( bool bWait )
{
    if( !bInit )
        return;
    
    pthread_mutex_lock( &Mutex );
    bPoolAlive = false;
    pthread_cond_broadcast(&Condition);
    pthread_mutex_unlock( &Mutex );
    
    if( bWait && (m_PoolCapacity > 0) )
    {
        pthread_mutex_lock (&Mutex);
        
        std::vector <Thread*>::iterator iter=m_threads.begin();
        for ( ;iter!=m_threads.end();)
        {
            pthread_mutex_unlock (&Mutex);
            pthread_join( (*iter)->pthread, NULL );
            pthread_mutex_lock (&Mutex);
            iter=m_threads.erase(iter);
        }
        
        pthread_mutex_unlock (&Mutex);
    }
    
    pthread_mutex_lock (&Mutex);
    if( bWait )
    {
        while( m_BurstThreadCount > 0 )
            pthread_cond_wait( &DetachCondition, &Mutex );
    }
    
    while(!m_JobQueue.empty())
    {
        delete m_JobQueue.front();
        m_JobQueue.pop();
    }
    
    pthread_mutex_unlock (&Mutex);
    
    bInit = false;
}

}
