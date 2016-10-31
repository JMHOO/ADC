#ifndef _ADCS__TCPSyncProcessor_H_
#define _ADCS__TCPSyncProcessor_H_

#include "GLog.h"
#include "ThreadPool.h"

class TCPSyncServerProcessor: public ADCS::IExecuteor
{
private:
    ILog* m_logger;
public:
    virtual bool Execute( void * lpvoid );
    
    virtual void SetLogger(ILog* logger) { m_logger = logger; }
    TCPSyncServerProcessor() {}

};

#endif
