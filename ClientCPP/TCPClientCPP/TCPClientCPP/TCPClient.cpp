#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "TCPClient.h"

CTCPClient::CTCPClient(): Socket(0), Port(0)
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

void CTCPClient::SetPort( unsigned short port )
{
    Port = port;
}


bool CTCPClient::Connect()
{
    memset( &ServerAddr, 0, sizeof(ServerAddr) );
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = inet_addr( sIP );
    ServerAddr.sin_port = htons( Port );
    
    if( ServerAddr.sin_addr.s_addr == INADDR_NONE )
        return false;
    
    Socket = socket( AF_INET, SOCK_STREAM, 0 );
    if( Socket < 0 )
        return false;
    
    if( connect( Socket, ( struct sockaddr * )&ServerAddr, sizeof( ServerAddr ) ) != 0 )
    {
        close( Socket );
        Socket = 0;
        return false;
    }
    
    return true;
}


int CTCPClient::SendInfo( char* sInfo, int nLen )
{
    char* pBuffer = sInfo;
    int	nBytesSent = 0;
    int nBytesTotal = nLen;
    
    ssize_t	nRet = 0;
    while(true)
    {
        nRet = send( Socket, pBuffer, nBytesTotal, 0 );
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
        nRet = recv( Socket, pBuffer, nBytesTotal, 0 );
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
    shutdown( Socket, 2 );
    close( Socket );
    Socket = 0;
    return true;
}
