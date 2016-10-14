

#ifndef TCPClient_h
#define TCPClient_h

#include "IClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class CTCPClient : public IClient
{
    int					Socket;
    struct sockaddr_in	ServerAddr;
    
    char				sIP[16];
    unsigned short		Port;
    
public:
    CTCPClient();
    
    virtual ~CTCPClient()
    {
        Close();
    }
    
    virtual bool SetIP( const char * ip );
    virtual void SetPort( unsigned short port );
    virtual void SetTimeOut( long second ) {};
    virtual void SetRetryTime( int iRetryTime ) {};
    
    virtual unsigned short GetPort() const
    {
        return Port;
    }
    virtual const char* GetIPAddr() const
    {
        return sIP;
    }
    
    virtual bool Connect( void );
    virtual int	SendInfo( char* sInfo, int nLen );
    virtual int	RecvInfo( char* sInfo, int nLen );
    virtual bool Close();
};

#endif
