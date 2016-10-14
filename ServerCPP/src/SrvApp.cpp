#include <stdio.h>
#include <iostream>
#include "SrvApp.h"
#include "TCPAsyncIOServer.h"
#include "UDPSyncIOServer.h"

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
    ILog* m_logger;
public:
    virtual bool Execute( void * pdata );
    bool Connect( ADCS::TCPConnParam* pCP );
    bool Recv   ( ADCS::TCPConnParam* pCP );
    bool Send   ( ADCS::TCPConnParam* pCP );
    bool Error  ( ADCS::TCPConnParam* pCP );
    
    TCPServerProcessor(ILog* logger) : m_logger(logger) {}
};

class UDPServerProcessor: public ADCS::IExecuteor
{
private:
    ILog* m_logger;
    bool ParseBuffer(char * buf, int len);
public:
    virtual bool Execute( void * pdata );
    
    UDPServerProcessor(ILog* logger) : m_logger(logger){}
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
        if( m_logger )m_logger->Warning("TCP Processor [Connect]: aio_read() failed, error code:%d, close session.", errno);
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
    char responseMsg[4096] = "TCPServer Response echo: ";
    strcat(responseMsg, pContext->sBuffer + sizeof(ADCS::PACK_HEADER));
    
    
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


bool UDPServerProcessor::ParseBuffer(char * buf, int len)
{
    ADCS::PACK_HEADER *pHeader = (ADCS::PACK_HEADER*)buf;
    
    if( ntohl(pHeader->Length) != (unsigned long)len )
    {
        if( ntohl(pHeader->Length) > (unsigned long)len )
        {
            if(m_logger)m_logger->Error("UDPServerProcessor Parser: received data less than required, drop package.");
        }
        else
        {
            if(m_logger)m_logger->Error("UDPServerProcessor Parser: received data larger than required, drop package.");
            
        }
        return false;
    }
    
    buf[ntohl(pHeader->Length)] = 0;
    
    // now we have package, deal with it.
    m_logger->Info("UDPServerProcessor Parser: got message from client -- %s", buf+sizeof(ADCS::PACK_HEADER));
   
    return true;
}

bool UDPServerProcessor::Execute( void * pdata )
{
    ADCS::CUDPSyncIOConnParam *pConnParam = (ADCS::CUDPSyncIOConnParam*)pdata;
    
    if( pConnParam == NULL )
    {
        if(m_logger)m_logger->Info("UDPServerProcessor::Execute, connection param error.");
        return false;
    }
    
    if( pConnParam->BytesTransferred == 0 )
    {
        if(m_logger)m_logger->Info("UDPServerProcessor::Execute, no data received, close session.");
        pConnParam->CloseSession();
        return true;
    }
    if( !ParseBuffer(pConnParam->Buffer, pConnParam->BytesTransferred))
    {
        if(m_logger)m_logger->Info("UDPServerProcessor::Execute, processing package error, close session.");
        pConnParam->CloseSession();
        return false;
    }
    
    
    // response to client
    char responseMsg[4096] = "UDPServer Response echo: ";
    strcat(responseMsg, pConnParam->Buffer + sizeof(ADCS::PACK_HEADER));

    int nSendDataLen = (int)(strlen(responseMsg) + sizeof(ADCS::PACK_HEADER));
    int nTransferedLen = 0;
    
    ADCS::PACK_HEADER header;
    header.Version = 100;
    header.Type = 0;
    header.Reserve = 0;
    header.Length = (unsigned int)htonl(nSendDataLen);

    memcpy(pConnParam->Buffer, &header, sizeof(ADCS::PACK_HEADER));
    memcpy(pConnParam->Buffer + sizeof(ADCS::PACK_HEADER), responseMsg, strlen(responseMsg));
    
    // Send it out
    nTransferedLen = pConnParam->Send( pConnParam->Buffer, nSendDataLen );
    if( nTransferedLen == 0 )
    {
        if(m_logger)m_logger->Info("UDPServerProcessor::Execute, sending data error.");
        pConnParam->CloseSession();
        return true;
    }
    else if( nTransferedLen != nSendDataLen )
    {
        if( nSendDataLen > nTransferedLen )
        {
            if(m_logger)m_logger->Error("UDPServerProcessor Execute: sent data less than required(excpet:%d, realsent:%d), close session.", nSendDataLen, nTransferedLen);
        }
        else
        {
            if(m_logger)m_logger->Error("UDPServerProcessor Execute: sent data larger than requiredexcpet:%d, realsent:%d), close session.", nSendDataLen, nTransferedLen);
            
        }
        pConnParam->CloseSession();
        return false;
    }
    
    // done, close session
    pConnParam->CloseSession();
    return true;
}







bool CServerApp::Start(unsigned short usPort)
{
    // Start TCP Server
    m_tcpProcessor = new TCPServerProcessor(m_pTcpLogger);
    m_pTcpThreadPool = new ADCS::CThreadPool();
    m_pTcpThreadPool->Init(*m_tcpProcessor, 10, 20, 100);
    
    m_tcpServer = new ADCS::CTCPAsyncIOServer();
    m_tcpServer->SetIP("0.0.0.0");
    m_tcpServer->SetPort(usPort);
    if(m_tcpServer->Start(m_pTcpThreadPool, m_pTcpLogger))
    {
        ADCS::CTCPAsyncIOServer* tcpserver = dynamic_cast<ADCS::CTCPAsyncIOServer*>(m_tcpServer);
        if( m_pTcpLogger)m_pTcpLogger->Info("TCP Server started: %s:%d LISTEN.", tcpserver->GetIP(), tcpserver->GetPort());
        
    }
    else
    {
        return false;
    }
    
    // Start UDP Server
    m_udpProcessor = new UDPServerProcessor(m_pUdpLogger);
    m_pUdpThreadPool = new ADCS::CThreadPool();
    m_pUdpThreadPool->Init(*m_udpProcessor, 10, 20, 100);
    
    m_udpServer = new ADCS::CUDPSyncIOServer();
    m_udpServer->SetIP("0.0.0.0");
    m_udpServer->SetPort(usPort+1);
    if( m_udpServer->Start(m_pUdpThreadPool, m_pUdpLogger))
    {
        ADCS::CUDPSyncIOServer* udpserver = dynamic_cast<ADCS::CUDPSyncIOServer*>(m_udpServer);
        if( m_pUdpLogger)m_pUdpLogger->Info("UDP Server started: %s:%d LISTEN.", udpserver->GetIP(), udpserver->GetPort());
       
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
    
    if( m_tcpProcessor)
    {
        delete m_tcpProcessor;
        m_tcpProcessor = NULL;
    }
    
    if( m_pTcpThreadPool)
    {
        m_pTcpThreadPool->Destory();
        delete m_pTcpThreadPool;
        m_pTcpThreadPool = NULL;
    }
    
    if( m_udpServer)
    {
        m_udpServer->Stop();
        delete m_udpServer;
        m_udpServer = NULL;
    }
    
    if( m_udpProcessor)
    {
        delete m_udpProcessor;
        m_udpProcessor = NULL;
    }
    
    if( m_pUdpThreadPool)
    {
        m_pUdpThreadPool->Destory();
        delete m_pUdpThreadPool;
        m_pUdpThreadPool = NULL;
    }
    
    return true;
}


CServerApp::CServerApp(): m_tcpServer(NULL), m_pTcpThreadPool(NULL), m_tcpProcessor(NULL),m_pTcpLogger(NULL),
    m_udpServer(NULL), m_pUdpThreadPool(NULL), m_udpProcessor(NULL), m_pUdpLogger(NULL)
{
    m_pTcpLogger = new GlobalLog("TCP", LL_DEBUG);
    m_pUdpLogger = new GlobalLog("UDP", LL_DEBUG);
}

CServerApp::~CServerApp()
{
    delete m_pTcpLogger;
    delete m_pUdpLogger;
}
