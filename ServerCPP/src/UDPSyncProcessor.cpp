#include <stdio.h>
#include <arpa/inet.h>
#include "UDPSyncProcessor.h"
#include "Network.h"
#include "UDPSyncIOServer.h"
#include "PackageInterface.h"

bool UDPServerProcessor::ParseBuffer(char * buf, int len)
{
    ADCS::PACK_HEADER *pHeader = (ADCS::PACK_HEADER*)buf;
    
    if( ntohl(pHeader->Length) != (unsigned long)len )
    {
        if( ntohl(pHeader->Length) > (unsigned long)len )
        {
            if(m_logger)m_logger->Error("UDPServerProcessor Parser: received data less than required, drop package. -- %s", buf + sizeof(ADCS::PACK_HEADER));
        }
        else
        {
            if(m_logger)m_logger->Error("UDPServerProcessor Parser: received data larger than required, drop package. -- %s", buf + sizeof(ADCS::PACK_HEADER));
            
        }
        return false;
    }
    
    //buf[ntohl(pHeader->Length)] = 0;
    
    
    return true;
}

bool UDPServerProcessor::Execute( void * pdata )
{
    ADCS::CUDPSyncIOConnParam *pConnParam = (ADCS::CUDPSyncIOConnParam*)pdata;
    
    if( pConnParam == NULL )
    {
        if(m_logger)m_logger->Info("UDP Server: connection param error.");
        return false;
    }
    
    if( pConnParam->BytesTransferred == 0 )
    {
        if(m_logger)m_logger->Info("UDP Server: no data received, close session.");
        pConnParam->CloseSession();
        return true;
    }
    if( !ParseBuffer(pConnParam->Buffer, pConnParam->BytesTransferred))
    {
        if(m_logger)m_logger->Info("UDP Server: processing package error, close session.");
        pConnParam->CloseSession();
        return false;
    }
    
    // now we have package, deal with it.
    if( m_logger)m_logger->Info("UDP Server: got message from client:%s -- %s", inet_ntoa(pConnParam->ClientIP.sin_addr), pConnParam->Buffer+sizeof(ADCS::PACK_HEADER));
    bool bRet = true;
    ADCS::PACK_HEADER *pHeader = (ADCS::PACK_HEADER*)pConnParam->Buffer;
    IPacket* packet = IPacket::CreatePackage(pConnParam->Buffer, ntohl(pHeader->Length), pConnParam->socketid);
    if( packet )
    {
        packet->Process();
        const char* ptrResponse = nullptr;
        unsigned long ulResponseLength = 0;
        packet->ToBytes(ptrResponse, ulResponseLength);
        
        if( m_logger)m_logger->Info("UDP Server: sending response to client:%s -- %s", inet_ntoa(pConnParam->ClientIP.sin_addr), ptrResponse+sizeof(ADCS::PACK_HEADER));
        
        // send whole package
        int nTransferedLen = pConnParam->Send( pConnParam->Buffer, (int)ulResponseLength );
        if( nTransferedLen == 0 )
        {
            if(m_logger)m_logger->Info("UDP Server: sending data failed, nothing sent.");
            pConnParam->CloseSession();
            bRet = true;
        }
        else if( nTransferedLen != (int)ulResponseLength )
        {
            if( (int)ulResponseLength > nTransferedLen )
            {
                if(m_logger)m_logger->Error("UDP Server: sent data less than required(excpet:%d, realsent:%d), close session.", ulResponseLength, nTransferedLen);
            }
            else
            {
                if(m_logger)m_logger->Error("UDP Server: sent data larger than requiredexcpet:%d, realsent:%d), close session.", ulResponseLength, nTransferedLen);
                
            }
            pConnParam->CloseSession();
            bRet = false;
        }
        
        delete packet;
    }
    else
    {
        if(m_logger)m_logger->Error("UDP Server create packet failed: invalid packet, dropped it. Content: %s", pConnParam->Buffer+sizeof(ADCS::PACK_HEADER));
    }
    
    pConnParam->CloseSession();
    return bRet;
    
    
    /* test code only
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
     */
}

