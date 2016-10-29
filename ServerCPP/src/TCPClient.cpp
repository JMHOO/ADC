#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
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
        return false;
    
    socketid = socket( AF_INET, SOCK_STREAM, 0 );
    if( socketid < 0 )
        return false;
    
    if( connect( socketid, ( struct sockaddr * )&serverAddr, sizeof( serverAddr ) ) != 0 )
    {
        close(socketid);
        socketid = 0;
        return false;
    }
    
    return true;
}


int CTCPClient::SendInfo( const char* sInfo, int nLen )
{
    const char* pBuffer = sInfo;
    int	nBytesSent = 0;
    int nBytesTotal = nLen;
    
    ssize_t	nRet = 0;
    while(true)
    {
        nRet = send( socketid, pBuffer, nBytesTotal, 0 );
        if( nRet > 0 )
        {
            nBytesSent += nRet;
            nBytesTotal -= nRet;
            if( nBytesTotal > 0 )
            {
                pBuffer += nRet;
                continue;
            }
            return nBytesSent;
        }
        else if( nRet == -1 )
        {
            if( errno == EINTR || errno == EAGAIN )
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
    int	nBytesReceived = 0;
    int nBytesTotal = nLen;
    
    ssize_t	nRet = 0;
    while(true)
    {
        nRet = recv( socketid, pBuffer, nBytesTotal, 0 );
        if( nRet > 0 )
        {
            nBytesReceived += nRet;
            nBytesTotal -= nRet;
            if( nBytesTotal > 0 )
            {
                pBuffer += nRet;
                continue;
            }
            return nBytesReceived;
        }
        else if( nRet == -1 )
        {
            if( errno == EINTR || errno == EAGAIN )
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
