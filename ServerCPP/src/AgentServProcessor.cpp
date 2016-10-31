#include <stdio.h>
#include <arpa/inet.h>
#include "AgentServProcessor.h"
#include "Network.h"
#include "TCPSyncIOServer.h"
#include "SocketHelper.h"
#include "PackageInterface.h"
#include "ServerManager.h"

bool AgentServerProcessor::Execute( void * pdata )
{
    ADCS::CTCPHelper tcpHelper;
    tcpHelper.SetTimeOut(20);
    tcpHelper.SetRetryTime(3);
    
    CServerManager* srvMgr = CServerManager::GetInstance();
    
    int	len = 0;
    int	recvedlen = 0;
    char sBuffer[1024];
    memset(sBuffer, 0, 1024);
    ADCS::PACK_HEADER *ptrHeader = (ADCS::PACK_HEADER *)sBuffer;
    
    ADCS::CTCPSIOConnParam *pConnParam = (ADCS::CTCPSIOConnParam*)pdata;
    
    if( pConnParam == NULL )
    {
        if(m_logger)m_logger->Info("Agent Server Processor: connection param error.");
        return false;
    }
    
    tcpHelper.SetSocketProperty(pConnParam->socketid);
    
    while(true)
    {
        memset(sBuffer, 0, 1024);
        
        // Receive
        recvedlen = tcpHelper.RecvInfo( pConnParam->socketid, sBuffer, (int)(sizeof(ADCS::PACK_HEADER)) );
        
        if( (int)(sizeof(ADCS::PACK_HEADER)) > recvedlen )
        {
            if( recvedlen == 0 )
            {
                if(m_logger)m_logger->Info("Agent Server: receive nothing, client might close.");
                
            }
            else
            {
                if(m_logger)m_logger->Error("Agent Server: receive package header error. Expect to receive %d bytes, in fact, it receive %d bytes -- %s", sizeof(ADCS::PACK_HEADER), recvedlen, sBuffer+sizeof(ADCS::PACK_HEADER));
            }
            
            // remove server
            srvMgr->UnregisterServer(pConnParam->socketid);
            
            tcpHelper.Close(pConnParam->socketid);
            delete pConnParam;
            
            return true;
        }
        
        int nRestDataLen = ntohl(ptrHeader->Length) - recvedlen;
        len = tcpHelper.RecvInfo( pConnParam->socketid, sBuffer + recvedlen, nRestDataLen );
        if( len != nRestDataLen )
        {
            if(m_logger)m_logger->Error("Agent Server: receive package payload error. Expect to receive %d bytes, in fact, it receive %d bytes -- %s", nRestDataLen, len, sBuffer+sizeof(ADCS::PACK_HEADER));

            // remove server
            srvMgr->UnregisterServer(pConnParam->socketid);

            tcpHelper.Close(pConnParam->socketid);
            delete pConnParam;
            
            return true;
        }
        
        
        // we got the whold package, deal with it
        if( m_logger)m_logger->Info("Agent Server: request from client[%d: %s] -- %s", pConnParam->socketid, inet_ntoa(pConnParam->ClientAddr.sin_addr), sBuffer+sizeof(ADCS::PACK_HEADER));

        IPacket* packet = IPacket::CreatePackage(sBuffer, ntohl(ptrHeader->Length), pConnParam->socketid);
        if( packet )
        {
            packet->Process();
            const char* ptrResponse = nullptr;
            unsigned long ulResponseLength = 0;
            packet->ToBytes(ptrResponse, ulResponseLength);
            
            //if( m_logger)m_logger->Info("Agent Server: sending response to client:%s -- %s", inet_ntoa(pConnParam->ClientAddr.sin_addr), ptrResponse+sizeof(ADCS::PACK_HEADER));
            
            // send whole package
            len = tcpHelper.SendInfo( pConnParam->socketid, ptrResponse, (int)ulResponseLength );
            if( (int)ulResponseLength != len )
            {
                if(m_logger)m_logger->Error("Agent Server: sending package header error. Expect to send %d bytes, in fact, it sent %d bytes", ulResponseLength, len);

                // remove server
                srvMgr->UnregisterServer(pConnParam->socketid);
                
                tcpHelper.Close(pConnParam->socketid);
                delete pConnParam;
                
                return true;
            }
            
            delete packet;
        }
        else
        {
            if(m_logger)m_logger->Error("Agent Server create packet failed: invalid packet, dropped it. Content: %s", sBuffer+sizeof(ADCS::PACK_HEADER));
        }
    }

    return true;
}
