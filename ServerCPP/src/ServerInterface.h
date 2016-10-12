
#ifndef _ADCS__ServerInterface_H_
#define _ADCS__ServerInterface_H_

#include "ThreadPool.h"
#include "GLog.h"

namespace ADCS
{
    class IServer
    {
    private:
        static void* ServerMain( void* pParam );

    protected:
        pthread_t   ServerMainThreadId;

    public:
        IServer() {}
        virtual ~IServer(){}
        
        virtual bool Start( ADCS::CThreadPool* pThreadPool, ILog * pLogger);
        virtual bool Stop();
        
        virtual bool Initialize(CThreadPool* pool, ILog *logger) = 0;
        virtual bool Main() = 0;
        virtual bool Close() = 0;
    };
}

#endif
