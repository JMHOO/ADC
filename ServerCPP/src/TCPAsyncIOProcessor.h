#ifndef _ADCS__TCPAsyncIOProcessor_H_
#define _ADCS__TCPAsyncIOProcessor_H_

#include "TCPAsyncIOServer.h"

class TCPAIOContext
{
public:
    static const int iBufferLength = 1024;
    
    ssize_t lTotal;
    ssize_t lCurrent;
    char sBuffer[iBufferLength];
    
    TCPAIOContext():lTotal(0), lCurrent(0)
    {
        memset( sBuffer, 0, iBufferLength );
    }
};

class TCPServerProcessor: public ADCS::IExecuteor
{
private:
    ILog* m_logger;
public:
    virtual bool Execute( void * pdata );
    bool Connect( ADCS::TCPConnParam* pCP );
    bool Recv   ( ADCS::TCPConnParam* pCP );
    bool Send   ( ADCS::TCPConnParam* pCP );
    bool Error  ( ADCS::TCPConnParam* pCP );
    
    TCPServerProcessor(ILog* logger) : m_logger(logger) {}
};


#endif
