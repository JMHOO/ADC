#include <stdio.h>
#include "SrvApp.h"
#include "TCPAsyncIOServer.h"

class TCPAIOContext
{
public:
    static const int iBufferLength = 1024;
    
    ssize_t		lTotal;
    ssize_t		lCurrent;
    char		sBuffer[iBufferLength];
    
    TCPAIOContext():lTotal(0), lCurrent(0)
    {
        memset( sBuffer, 0, iBufferLength );
    }
};


class TCPServerProcessor: public ADCS::IExecuteor
{
public:
    virtual bool Execute( void * pdata );
    bool Connect( ADCS::TCPConnParam* pCP );
    bool Recv   ( ADCS::TCPConnParam* pCP );
    bool Send   ( ADCS::TCPConnParam* pCP );
    bool Error  ( ADCS::TCPConnParam* pCP );
};

bool TCPServerProcessor::Execute( void * pdata )
{
    ADCS::TCPConnParam *pConnParam = static_cast<ADCS::TCPConnParam*>(pdata);
    if( pConnParam == NULL )
    {
        
        return false;
    }
    
    switch( pConnParam->Event )
    {
        case ADCS::Events::EV_Connect:
            return Connect( pConnParam );
        case ADCS::Events::EV_Recv:
            return Recv( pConnParam );
        case ADCS::Events::EV_Send:
            return Send( pConnParam );
        case ADCS::Events::EV_Exit:
        case ADCS::Events::EV_ThreadPoolError:
        default:
            return Error( pConnParam );
    }
    
    return true;
}

bool TCPServerProcessor::Connect( ADCS::TCPConnParam* pCP )
{
    return true;
}
bool TCPServerProcessor::Recv( ADCS::TCPConnParam* pCP )
{
    return true;
}
bool TCPServerProcessor::Send( ADCS::TCPConnParam* pCP )
{
    return true;
}
bool TCPServerProcessor::Error( ADCS::TCPConnParam* pCP )
{
    return true;
}

bool CServerApp::Start(unsigned short usPort)
{
    m_processor = new TCPServerProcessor();
    m_pTcpThreadPool = new ADCS::CThreadPool();
    m_pTcpThreadPool->Init(*m_processor, 10, 20, 100);
    
    m_tcpServer = new ADCS::CTCPAsyncIOServer();
    if(m_tcpServer->Start(m_pTcpThreadPool, m_pLogger))
    {
        
    }
    else
    {
        return false;
    }
    
    return true;
}

bool CServerApp::Stop()
{
    if( m_tcpServer)
    {
        m_tcpServer->Stop();
        delete m_tcpServer;
        m_tcpServer = NULL;
    }
    
    if( m_processor)
    {
        delete m_processor;
        m_processor = NULL;
    }
    
    if( m_pTcpThreadPool)
    {
        m_pTcpThreadPool->Destory();
        delete m_pTcpThreadPool;
        m_pTcpThreadPool = NULL;
    }
    
    return true;
}


CServerApp::CServerApp(): m_tcpServer(NULL), m_processor(NULL), m_pTcpThreadPool(NULL), m_pLogger(NULL)
{
    m_pLogger = new GlobalLog("TCP", LL_DEBUG);
}

CServerApp::~CServerApp()
{
    delete m_pLogger;
}
