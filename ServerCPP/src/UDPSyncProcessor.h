#ifndef _ADCS__UDPSyncProcessor_H_
#define _ADCS__UDPSyncProcessor_H_

#include "GLog.h"
#include "ThreadPool.h"

class UDPServerProcessor: public ADCS::IExecuteor
{
private:
    ILog* m_logger;
public:
    virtual bool Execute( void * lpvoid );
    
    bool ParseBuffer(char * buf, int len);
    UDPServerProcessor(ILog* logger) : m_logger(logger) {}
    
};

#endif
