
#ifndef __ADC__IClient_H__
#define __ADC__IClient_H__

class IClient
{
public:
    virtual              ~IClient()	{}
    
    virtual bool			SetIP(const char * ip) = 0;
    virtual void			SetPort(unsigned short port) = 0;
    virtual void			SetTimeOut(int second) = 0;
    virtual void			SetRetryTime(int time) = 0;
    
    virtual unsigned short GetPort() const = 0;
    virtual const char*	GetIPAddr() const = 0;
    
    virtual bool			Connect() = 0;
    virtual int			SendInfo(const char* sInfo, int nLen ) = 0;
    virtual int			RecvInfo(char* sInfo, int nLen ) = 0;
    virtual bool			Close() = 0;
};

#endif
