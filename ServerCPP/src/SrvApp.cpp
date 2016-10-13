#include <stdio.h>
#include <iostream>
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
private:
    ILog*       m_logger;
public:
    virtual bool Execute( void * pdata );
    bool Connect( ADCS::TCPConnParam* pCP );
    bool Recv   ( ADCS::TCPConnParam* pCP );
    bool Send   ( ADCS::TCPConnParam* pCP );
    bool Error  ( ADCS::TCPConnParam* pCP );
    
    TCPServerProcessor(ILog* logger) : m_logger(logger) {}
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
    TCPAIOContext *pContext = new TCPAIOContext;
    if( !pContext )
    {
        if( m_logger )m_logger->Warning("TCP Processor [Connect]: TCPAIOContext create failed, close session.");
        pCP->CloseSession();
        return false;
    }
    
    pCP->Context = (void *)pContext;
    pContext->lTotal = (ssize_t)sizeof(ADCS::PACK_HEADER);
    pContext->lCurrent = 0;
    
    pCP->Event = ADCS::Events::EV_Recv;
    pCP->SetAIOBuffer(pContext->sBuffer, pContext->lTotal);
    
    //-- read
    if( aio_read( &(pCP->AIOControlBlock) ) == -1 )
    {
        if( m_logger )m_logger->Warning("TCP Processor [Connect]: aio_read() failed, close session.");
        delete pContext;
        pCP->CloseSession();
        return false;
    }
    
    return true;
}

bool TCPServerProcessor::Recv( ADCS::TCPConnParam* pCP )
{
    TCPAIOContext *pContext = (TCPAIOContext *)pCP->Context;
    if( !pContext )
    {
        if( m_logger )m_logger->Warning("TCP Processor [Recv]: nothing in Context, close session.");
        pCP->CloseSession();
        return false;
    }
    
    ssize_t	nRead = 0;
    ADCS::PACK_HEADER *pHeader = (ADCS::PACK_HEADER *)pContext->sBuffer;
    
    nRead = aio_return( &(pCP->AIOControlBlock) );
    if( nRead <= 0 )
    {
        if( nRead == -1 )
        {
            if( errno == EINTR || errno == EAGAIN )
            {
                if( aio_read( &(pCP->AIOControlBlock) ) == -1 )
                {
                    if( m_logger )m_logger->Warning("TCP Processor [Recv]: aio_read() failed, close session.");
                    delete pContext;
                    pCP->CloseSession();
                    return false;
                }
                return true;
            }
        }
        
        if( nRead == 0 )
        {
            if( m_logger )m_logger->Info("TCP Processor [Recv]: Client exited, close session.");
        }
        else
        {
            if( m_logger )m_logger->Warning("TCP Processor [Recv]: aio_return() failed, close session.");
        }
        delete pContext;
        pCP->CloseSession();
        return false;
    }
    
    pContext->lCurrent += nRead;
    
    if(  pContext->lTotal == (ssize_t)sizeof(ADCS::PACK_HEADER) )
        pContext->lTotal = (ssize_t)ntohl(pHeader->Length);
    
    size_t iExpectedLength = (size_t)( pContext->lTotal - pContext->lCurrent);
    
    if( iExpectedLength > 0 )
    {
        pCP->SetAIOBuffer( pContext->sBuffer + pContext->lCurrent, iExpectedLength );
        if( aio_read( &(pCP->AIOControlBlock) ) == -1 )
        {
            if( m_logger )m_logger->Warning("TCP Processor [Recv]: aio_read() failed, close session.");
            delete pContext;
            pCP->CloseSession();
            return false;
        }
        return true;
    }
    
    // processing package
    pContext->sBuffer[pContext->lTotal] = 0;
    
    if(m_logger)m_logger->Info("Received Message: %s", pContext->sBuffer + sizeof(ADCS::PACK_HEADER));

    
    
    // Response to client
    char responseMsg[] = "this is a response from server";
    
    pCP->Event = ADCS::Events::EV_Send;
    pContext->lTotal = (ssize_t)(sizeof(ADCS::PACK_HEADER) + strlen(responseMsg));
    pContext->lCurrent = 0;
    
    pHeader->Length = ntohl(pContext->lTotal);
    memcpy( pContext->sBuffer + sizeof(ADCS::PACK_HEADER), responseMsg, strlen(responseMsg) );
    
    pCP->SetAIOBuffer(pContext->sBuffer, pContext->lTotal );
    if( aio_write( &(pCP->AIOControlBlock) ) == -1 )
    {
        if( m_logger )m_logger->Warning("TCP Processor [Recv]: aio_write() failed, close session.");
        delete pContext;
        pCP->CloseSession();
        return false;
    }
    return true;
}

bool TCPServerProcessor::Send( ADCS::TCPConnParam* pCP )
{
    TCPAIOContext *pContext = (TCPAIOContext *)pCP->Context;
    if( !pContext )
    {
        if(m_logger)m_logger->Error("TCP Processor [Send]: Context is NULL, close session.");
        pCP->CloseSession();
        return false;
    }
    
    ssize_t nRead = aio_return( &(pCP->AIOControlBlock) );
    if( nRead <= 0 )
    {
        if( nRead == -1 )
        {
            if( errno == EINTR || errno == EAGAIN )
            {
                if( aio_write( &(pCP->AIOControlBlock) ) == -1 )
                {
                    if( m_logger)m_logger->Error("TCP Processor [Send]: aio_write() failed, send error, close session.");
                    pCP->CloseSession();
                    return false;
                }
                return true;
            }
        }
        
        if( nRead == 0 )
        {
            if(m_logger)m_logger->Info("TCP Processor [Send]: Client exited, close session.");
        }
        else
        {
            if(m_logger)m_logger->Info("TCP Processor [Send]: aio_return() failed, close session.");
        }

        delete pContext;
        pCP->CloseSession();
        return false;
    }
    
    delete pContext;
    pCP->CloseSession();
    
    return true;
}

bool TCPServerProcessor::Error( ADCS::TCPConnParam* pCP )
{
    TCPAIOContext *pContext = (TCPAIOContext *)pCP->Context;
    
    switch( pCP->Event )
    {
        case ADCS::Events::EV_Exit:
            if(m_logger)m_logger->Error("TCP Processor [Error]: Exit.");
            break;
        case ADCS::Events::EV_ThreadPoolError:
            if(m_logger)m_logger->Error("TCP Processor [Error]: Thread Pool Error.");
            break;
        default:
            if(m_logger)m_logger->Error("TCP Processor [Error]: Unknown Error.");
            break;
    }
    
    delete pContext;
    pCP->CloseSession();
    
    return true;
}





bool CServerApp::Start(unsigned short usPort)
{
    m_processor = new TCPServerProcessor(m_pLogger);
    m_pTcpThreadPool = new ADCS::CThreadPool();
    m_pTcpThreadPool->Init(*m_processor, 10, 20, 100);
    
    m_tcpServer = new ADCS::CTCPAsyncIOServer();
    m_tcpServer->SetIP("0.0.0.0");
    m_tcpServer->SetPort(usPort);
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


CServerApp::CServerApp(): m_tcpServer(NULL), m_pTcpThreadPool(NULL), m_pLogger(NULL),m_processor(NULL)
{
    m_pLogger = new GlobalLog("TCP", LL_DEBUG);
}

CServerApp::~CServerApp()
{
    delete m_pLogger;
}
