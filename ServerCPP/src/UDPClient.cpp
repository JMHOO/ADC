#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "UDPClient.h"

CUDPClient::CUDPClient(): socketid(0), port(0), bConnected(false)
{
    strcpy(sIP, "127.0.0.1");
}

bool CUDPClient::SetIP( const char *ip )
{
    if( strlen(ip) >= 16 )
        return false;
    
    strcpy( sIP, ip );
    return true;
}

void CUDPClient::SetPort( unsigned short sport )
{
    port = sport;
}

bool CUDPClient::Connect()
{
    memset( &servAddr, 0, sizeof(servAddr) );
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(sIP);
    servAddr.sin_port = htons( port );
    
    if( servAddr.sin_addr.s_addr == INADDR_NONE )
    {
        // try dns resolve
        char ip[32] = {0};
        
        struct hostent *host;
        struct in_addr **addr_list;
        host = gethostbyname(sIP);
        addr_list = (struct in_addr **)host->h_addr_list;
        
        strcpy(ip, inet_ntoa(*addr_list[0]));
        
        servAddr.sin_addr.s_addr = inet_addr(ip);
        if( servAddr.sin_addr.s_addr == INADDR_NONE )
            return false;
    }
    
    if( servAddr.sin_addr.s_addr == INADDR_NONE )
        return false;
    
    socketid = socket( AF_INET, SOCK_DGRAM, 0 );
    if( socketid < 0 )
        return false;
    
    if( bConnected )
    {
        if( connect( socketid, (struct sockaddr *)&servAddr, sizeof(servAddr) ) != 0 )
        {
            close( socketid );
            socketid = 0;
            return false;
        }
    }
    
    return true;
}

int CUDPClient::SendInfo( const char* szInfo, int iLen )
{
    if( bConnected )
        return (int)send( socketid, szInfo, iLen, 0 );
    else
        return (int)sendto( socketid, szInfo, iLen, 0, (struct sockaddr *)&servAddr, sizeof(servAddr) );
}


int CUDPClient::RecvInfo( char* szInfo, int iLen )
{
    if( bConnected )
        return (int)recv( socketid, szInfo, iLen, 0 );
    else
    {
        socklen_t AddrStrLen = sizeof( servAddr );
        return (int)recvfrom( socketid, szInfo, iLen, 0, (struct sockaddr *)&servAddr, &AddrStrLen );
    }
}

bool CUDPClient::Close()
{
    if( socketid > 0 )
        close( socketid );
    socketid = 0;
    return true;
}
