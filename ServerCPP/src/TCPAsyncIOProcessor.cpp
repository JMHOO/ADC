#include <stdio.h>
#include "TCPAsyncIOProcessor.h"

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

