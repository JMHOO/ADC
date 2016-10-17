#ifndef _ADCS__SocketHelper_H_
#define _ADCS__SocketHelper_H_

namespace ADCS
{
    class CTCPHelper
    {
    protected:
        int	nTimeOut;
        int	nRetryTime;
        
    public:
        int	SendInfoRaw( int Socket, const char* szInfo, int iLen );
        int	RecvInfoRaw( int Socket, char* szInfo, int iLen );
        int	SendInfo( int Socket, const char* szInfo, int iLen );
        int	RecvInfo( int Socket, char* szInfo, int iLen );
        bool SetSocketProperty( int Socket );
        
    public:
        CTCPHelper(): nTimeOut(5), nRetryTime(3) { }
        ~CTCPHelper() { }
        
        void SetTimeOut( int second = 5 );
        void SetRetryTime( int nRetryTime = 3 );
        
        bool Close( int Socket );
    };
}

#endif
