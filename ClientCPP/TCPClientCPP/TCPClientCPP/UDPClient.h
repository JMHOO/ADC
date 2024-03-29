
#ifndef __UDPClient_H_
#define __UDPClient_H_

#include "IClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class CUDPClient : public IClient
{
private:
    int socketid;
    struct sockaddr_in servAddr;
    
    char sIP[16];
    unsigned short port;
    bool bConnected;
    
public:
    CUDPClient();
    
    virtual ~CUDPClient()
    {
        Close();
    }
    
    virtual bool SetIP( const char * ip );
    virtual void SetPort( unsigned short sport );
    virtual void SetTimeOut( long second ) {};
    virtual void SetRetryTime( int iRetryTime ) {};
    virtual void SetConnect( bool bConn = false ) { this->bConnected = bConn; };
    virtual unsigned short GetPort() const {return port;}
    virtual const char* GetIPAddr() const {return sIP; }
    
    virtual bool Connect();
    virtual int	SendInfo(char* sInfo, int nLen);
    virtual int	RecvInfo(char* sInfo, int nLen);
    virtual bool Close();
};

#endif
