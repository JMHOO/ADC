/*===========================================================================
 FILE: ThreadPool.h
 ===========================================================================*/
#ifndef _ADCS__THREAD_POOL_H_
#define _ADCS__THREAD_POOL_H_


#include <pthread.h>
#include <queue>
#include <list>
#include "ErrorCode.h"
#include "GLog.h"

namespace ADCS
{
    //Interface of the job class.
    class IExecuteor
    {
    public:
        virtual bool Execute( void * data ) = 0;
        virtual void SetLogger(ILog* logger) = 0;
        virtual ~IExecuteor()	{}
    };
    
    class CThreadPool
    {
    private:
        typedef struct _job
        {
            void* data;                 // job's payload
        }Job, *PJob;
        
        typedef struct _thread
        {
            int id;
            bool bBurst;                // if thread count larger than we expected, it will be marked as a burst thread and will be destoried when thread exit.
            pthread_t pthread;
            CThreadPool *pool;
        }Thread;
        
    
        pthread_mutex_t			Mutex;
        pthread_cond_t			Condition;
        pthread_cond_t			DetachCondition;
        
        unsigned int			m_MaxThreadCount;   // the maximum thread count can not be beyonded.
        unsigned int            m_PoolCapacity;     // if threads count >= pool's capacity, they will be marked as burst thread
        unsigned int            m_ThreadCount;          // the count of threads which are not marked as burst thread
        unsigned int            m_WorkingThreadCount;   // working or busy threads
        unsigned int            m_BurstThreadCount;     // the count of burst threads
        unsigned int            m_eachIncreaseCount;    // each time we supplement the count of threads, this var will equal to initil count of threads
        
        std::queue<PJob>		m_JobQueue;
        std::vector<Thread*>    m_threads;          // threads that hold by pool.
        
        IExecuteor				*m_ptrExecutor;
        
        bool					bInit;
        bool					bPoolAlive;
        
        bool					_initialize( unsigned int uiInitialCount, unsigned int uiCapacity, unsigned int uiMaxCount);
        bool					SupplementThreads();
        bool					AppendThread(bool bBurst);

        static void*            _thread_run_( void* lpParam );
        
    public:
        bool    Init( IExecuteor &executor, unsigned int uiInitialCount, unsigned int uiCapacity, unsigned int uiMaxCount);
        bool    WakeUp( void * data );
        void	Destory( bool bWait = true );
        void	GetStatus( unsigned int &uiThreadCount, unsigned int &uiBurstThreadCount, unsigned int &uiWorkingThreadCount, unsigned int &uiJobCount );
        void	GetStatusWithoutLock( unsigned int &uiThreadCount, unsigned int &uiBurstThreadCount, unsigned int &uiWorkingThreadCount, unsigned int &uiJobCount);
        void	SetExecutor(IExecuteor &executor ) { m_ptrExecutor = &executor; }
        bool	SetExecutor(IExecuteor *executor)
        {
            if(!executor)
                return false;
            
            m_ptrExecutor = executor;
            return true;
        }
        
        IExecuteor*	GetExecutor() { return m_ptrExecutor; }
        bool IsInit() { return bInit; }
        ~CThreadPool()
        {
            Destory();
            pthread_mutex_destroy(&Mutex);
            pthread_cond_destroy(&Condition);
            pthread_cond_destroy(&DetachCondition);
        }
    };
}
#endif
