#include <stdio.h>
#include "TCPSyncProcessor.h"
#include "Network.h"
#include "TCPSyncIOServer.h"
#include "SocketHelper.h"
#include "PackageInterface.h"

bool TCPSyncServerProcessor::Execute( void * pdata )
{
    ADCS::CTCPHelper tcpHelper;
    
    int	len = 0;
    int	recvedlen = 0;
    char sBuffer[1024];
    ADCS::PACK_HEADER *ptrHeader = (ADCS::PACK_HEADER *)sBuffer;
    
    ADCS::CTCPSIOConnParam *pConnParam = (ADCS::CTCPSIOConnParam*)pdata;
    
    if( pConnParam == NULL )
    {
        if(m_logger)m_logger->Info("TCPSyncServerProcessor::Execute, connection param error.");
        return false;
    }
    
    tcpHelper.SetSocketProperty(pConnParam->socketid);
    
    // Receive
    recvedlen = tcpHelper.RecvInfo( pConnParam->socketid, sBuffer, (int)(sizeof(ADCS::PACK_HEADER)) );
    
    if( (int)(sizeof(ADCS::PACK_HEADER)) > recvedlen )
    {
        if( recvedlen == 0 )
        {
            if(m_logger)m_logger->Info("TCPSyncServerProcessor::Execute, receive nothing, client might closed.");
        }
        else
        {
            if(m_logger)m_logger->Error("TCPSyncServerProcessor::Execute, receive package header error. Expect to receive %d bytes, in fact, it receive %d bytes", sizeof(ADCS::PACK_HEADER), recvedlen);
        }
        tcpHelper.Close(pConnParam->socketid);
        delete pConnParam;
        return true;
    }
    
    int nRestDataLen = ntohl(ptrHeader->Length) - recvedlen;
    len = tcpHelper.RecvInfo( pConnParam->socketid, sBuffer + recvedlen, nRestDataLen );
    if( len != nRestDataLen )
    {
        if(m_logger)m_logger->Error("TCPSyncServerProcessor::Execute, receive package payload error. Expect to receive %d bytes, in fact, it receive %d bytes", nRestDataLen, len);
        tcpHelper.Close(pConnParam->socketid);
        delete pConnParam;
        return true;
    }
    
    
    // we got the whold package, deal with it
    IPacket* packet = IPacket::CreatePackage(sBuffer, ntohl(ptrHeader->Length), pConnParam->socketid);
    if( packet )
    {
        packet->Process();
        const char* ptrResponse = nullptr;
        unsigned long ulResponseLength = 0;
        packet->ToBytes(ptrResponse, ulResponseLength);
        
        // send whole package
        len = tcpHelper.SendInfo( pConnParam->socketid, ptrResponse, (int)ulResponseLength );
        if( (int)ulResponseLength != len )
        {
            if(m_logger)m_logger->Error("TCPSyncServerProcessor::Execute, sending package header error. Expect to send %d bytes, in fact, it sent %d bytes", ulResponseLength, len);
            tcpHelper.Close(pConnParam->socketid);
            delete pConnParam;
            //return true;
        }
        
        delete packet;
    }
    else
    {
        if(m_logger)m_logger->Error("TCPSyncServer Invalid packet, dropped it. Content: %s", sBuffer+sizeof(ADCS::PACK_HEADER));
    }
    
    /* test code, just echo response
    // response to client
    char responseMsg[4096] = "TCPServer Response echo: ";
    strcat(responseMsg, sBuffer + sizeof(ADCS::PACK_HEADER));
    
    int nSendDataLen = (int)(strlen(responseMsg) + sizeof(ADCS::PACK_HEADER));
    
    ADCS::PACK_HEADER header;
    header.Version = 100;
    header.Type = 0;
    header.Reserve = 0;
    header.Length = (unsigned int)htonl(nSendDataLen);
    
    //memcpy(sBuffer, &header, sizeof(ADCS::PACK_HEADER));
    //memcpy(sBuffer + sizeof(ADCS::PACK_HEADER), responseMsg, strlen(responseMsg));
    
    // send it out
    len = tcpHelper.SendInfo( pConnParam->socketid, (char *)&header, (int)(sizeof(ADCS::PACK_HEADER)) );
    if( (int)(sizeof(ADCS::PACK_HEADER)) != len )
    {
        if(m_logger)m_logger->Error("TCPSyncServerProcessor::Execute, sending package header error. Expect to send %d bytes, in fact, it receive %d bytes", sizeof(ADCS::PACK_HEADER), len);
        tcpHelper.Close(pConnParam->socketid);
        delete pConnParam;
        return true;
    }
    
    // send payload
    len = tcpHelper.SendInfo( pConnParam->socketid, responseMsg, (int)strlen(responseMsg) );
    if( (int)strlen(responseMsg) != len )
    {
        if(m_logger)m_logger->Error("TCPSyncServerProcessor::Execute, sending package payload error. Expect to send %d bytes, in fact, it receive %d bytes", strlen(responseMsg), len);
        tcpHelper.Close(pConnParam->socketid);
        delete pConnParam;
        return true;
    }
     */

    return true;
}
