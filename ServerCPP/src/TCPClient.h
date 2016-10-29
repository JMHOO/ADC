

#ifndef __TCPClient_H_
#define __TCPClient_H_

#include "IClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class CTCPClient : public IClient
{
private:
    int socketid;
    struct sockaddr_in serverAddr;
    
    char sIP[16];
    unsigned short port;
    
public:
    CTCPClient();
    
    virtual ~CTCPClient()
    {
        Close();
    }
    
    virtual bool SetIP( const char * ip );
    virtual void SetPort( unsigned short sport );
    virtual void SetTimeOut( long second ) {};
    virtual void SetRetryTime( int iRetryTime ) {};
    
    virtual unsigned short GetPort() const
    {
        return port;
    }
    
    virtual const char* GetIPAddr() const
    {
        return sIP;
    }
    
    virtual bool Connect( void );
    virtual int	SendInfo( const char* sInfo, int nLen );
    virtual int	RecvInfo( char* sInfo, int nLen );
    virtual bool Close();
};

#endif
