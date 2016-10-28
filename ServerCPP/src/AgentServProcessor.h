#ifndef _ADCS__AgentServerProcessor_H_
#define _ADCS__AgentServerProcessor_H_

#include "GLog.h"
#include "ThreadPool.h"

class AgentServerProcessor: public ADCS::IExecuteor
{
private:
    ILog* m_logger;
public:
    virtual bool Execute( void * lpvoid );
    
    
    AgentServerProcessor(ILog* logger) : m_logger(logger) {}

};

#endif
