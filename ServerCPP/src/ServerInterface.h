
#ifndef _ADCS__ServerInterface_H_
#define _ADCS__ServerInterface_H_

#include <string>
#include "ThreadPool.h"
#include "GLog.h"

namespace ADCS
{
    class IServer
    {
    public:
        static IServer* CreateServer(const char* sName);
        
    private:
        static void* ServerMain( void* pParam );

    protected:
        pthread_t   ServerMainThreadId;
        std::string m_serverName;
        CThreadPool *m_pThreadPool;
        ILog        *m_logger;
        IExecuteor  *m_processor;

    public:
        IServer(const char* sName, IExecuteor* executor);
        virtual ~IServer();
        
        virtual bool Start(unsigned short usPort);
        virtual bool Stop();
        virtual ILog* GetLogger();
        
        virtual bool Initialize() = 0;
        virtual bool Main() = 0;
        virtual bool Close() = 0;
        virtual bool SetIP(const char *ip) = 0;
        virtual void SetPort(const unsigned short port) = 0;
        virtual const char* GetIP() = 0;
        virtual unsigned short GetPort() = 0;
    };
}

#endif
