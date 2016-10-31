#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "TCPClient.h"

CTCPClient::CTCPClient(): socketid(0), port(0)
{
    strcpy( sIP, "127.0.0.1" );
}

bool CTCPClient::SetIP( const char *ip )
{
    if( strlen(ip) >= 16 )
        return false;
    
    strcpy( sIP, ip );
    return true;
}

void CTCPClient::SetPort( unsigned short sport )
{
    port = sport;
}


bool CTCPClient::Connect()
{
    memset( &serverAddr, 0, sizeof(serverAddr) );
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr( sIP );
    serverAddr.sin_port = htons( port );
    
    if( serverAddr.sin_addr.s_addr == INADDR_NONE )
    {
        // try dns resolve
        char ip[32] = {0};
        
        struct hostent *host;
        struct in_addr **addr_list;
        host = gethostbyname(sIP);
        addr_list = (struct in_addr **)host->h_addr_list;
        
        strcpy(ip, inet_ntoa(*addr_list[0]));
        
        serverAddr.sin_addr.s_addr = inet_addr(ip);
        if( serverAddr.sin_addr.s_addr == INADDR_NONE )
            return false;
    }
    
    socketid = socket( AF_INET, SOCK_STREAM, 0 );
    if( socketid < 0 )
        return false;
    
    if( connect(socketid, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0 )
    {
        close( socketid );
        socketid = 0;
        return false;
    }
    
    int flags = fcntl( socketid, F_GETFL );
    if( flags == -1 )
    {
        Close();
        return false;
    }
    
    if( fcntl( socketid, F_SETFL, flags | O_NDELAY ) != 0 )
    {
        Close();
        return false;
    }
    
    return true;
}

int CTCPClient::SendInfoRaw(const char* sInfo, int nLen)
{  
    fd_set fdWrite;
    int iRet = 0;
    
    struct timeval tv;
    tv.tv_sec = m_timeout;
    tv.tv_usec = 0;
    
    FD_ZERO( &fdWrite );
    FD_SET( socketid, &fdWrite );
    iRet = select( socketid + 1, NULL, &fdWrite, NULL, &tv );
    
    if( iRet > 0 && FD_ISSET( socketid, &fdWrite ) )
        return (int)send( socketid, sInfo, nLen, 0 );
    
    return iRet;

}

int CTCPClient::RecvInfoRaw(char* sInfo, int nLen)
{
    fd_set fdRead;
    int 	iRet = 0;
    
    struct timeval tv;
    tv.tv_sec = m_timeout;
    tv.tv_usec = 0;
    
    FD_ZERO( &fdRead );
    FD_SET( socketid, &fdRead );
    iRet = select( socketid + 1, &fdRead, NULL, NULL, &tv );
    
    if( iRet > 0 && FD_ISSET( socketid, &fdRead ) )
        return (int)recv( socketid, sInfo, nLen, 0 );
    
    return iRet;
}

int CTCPClient::SendInfo( const char* sInfo, int nLen )
{
    const char* pBuffer = sInfo;
    int nBytesTransferred = 0;
    int nBytesForTransfer = nLen;
    
    int nRet = 0;
    for( int i = 0; i < m_retry; i++ )
    {
        nRet = SendInfoRaw( pBuffer, nBytesForTransfer );
        if( nRet > 0 )
        {
            nBytesTransferred += nRet;
            nBytesForTransfer -= nRet;
            if( nBytesForTransfer > 0 )
            {
                i = 0;
                pBuffer += nRet;
                continue;
            }
            return nBytesTransferred;
        }
        else if( nRet == -1 )
        {
            if( i < m_retry - 1 )
                continue;
            else
                return -1;
        }
        return 0;
    }
    
    return 0;
}

int CTCPClient::RecvInfo( char* sInfo, int nLen )
{
    char* pBuffer = sInfo;
    int nBytesTransferred = 0;
    int nBytesForTransfer = nLen;
    
    int nRet = 0;
    for( int i = 0; i < m_retry; i++ )
    {
        nRet = RecvInfoRaw(pBuffer, nBytesForTransfer);
        if( nRet > 0 )
        {
            nBytesTransferred += nRet;
            nBytesForTransfer -= nRet;
            if( nBytesForTransfer > 0 )
            {
                i = 0;
                pBuffer += nRet;
                continue;
            }
            return nBytesTransferred;
        }
        else if( nRet == -1 )
        {
            if( i < m_retry - 1 )
                continue;
            else
                return -1;
        }
        return 0;
    }
    
    return 0;
}


bool CTCPClient::Close()
{
    shutdown( socketid, 2 );
    close( socketid );
    socketid = 0;
    return true;
}

int CTCPClient::GetSocketID()
{
    return socketid;
}
