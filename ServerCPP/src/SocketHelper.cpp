#include "SocketHelper.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

namespace ADCS
{
    void CTCPHelper::SetTimeOut( int second )
    {
        nTimeOut = second;
    }

    void CTCPHelper::SetRetryTime( int nRetryTime )
    {
        if( nRetryTime > 0 )
            this->nRetryTime = nRetryTime;
        else
            this->nRetryTime = 1;
    }

    bool CTCPHelper::SetSocketProperty( int Socket )
    {
        int flags = fcntl( Socket, F_GETFL );
        if( flags == -1 )
            return false;
        
        if( fcntl( Socket, F_SETFL, flags | O_NDELAY ) != 0 )
        {
            return false;
        }
        
        return true;
    }

    int CTCPHelper::SendInfoRaw( int Socket, char* szInfo, int iLen )
    {
        struct timeval tv;
        fd_set fdWrite;
        int iRet = 0;
        
        tv.tv_sec = nTimeOut;
        tv.tv_usec = 0;
        
        FD_ZERO( &fdWrite );
        FD_SET( Socket, &fdWrite );
        iRet = select( Socket + 1, NULL, &fdWrite, NULL, &tv );
        
        if( iRet > 0 && FD_ISSET( Socket, &fdWrite ) )
            return (int)send( Socket, szInfo, iLen, 0 );
        
        return iRet;
    }

    int CTCPHelper::RecvInfoRaw( int Socket, char* szInfo, int iLen )
    {
        fd_set				fdRead;
        struct timeval		tv;
        int					iRet = 0;
        
        tv.tv_sec = nTimeOut;
        tv.tv_usec = 0;
        
        FD_ZERO( &fdRead );
        FD_SET( Socket, &fdRead );
        iRet = select( Socket + 1, &fdRead, NULL, NULL, &tv );
        
        if( iRet > 0 && FD_ISSET( Socket, &fdRead ) )
            return (int)recv( Socket, szInfo, iLen, 0 );
        
        return iRet;
    }

    int CTCPHelper::SendInfo( int Socket, char* szInfo, int iLen )
    {
        char* pBuffer = szInfo;
        int	nBytesTransferred = 0;
        int	nBytesForTransfer = iLen;
        
        for( int i = 0, iRet = 0; i < nRetryTime; i++ )
        {
            iRet = SendInfoRaw( Socket, pBuffer, nBytesForTransfer );
            if( iRet > 0 )
            {
                nBytesTransferred += iRet;
                nBytesForTransfer -= iRet;
                if( nBytesForTransfer > 0 )
                {
                    i = 0;
                    pBuffer += iRet;
                    continue;
                }
                return nBytesTransferred;
            }
            else if( iRet == -1 )
            {
                if( i < nRetryTime - 1 )
                    continue;
                else
                    return -1;
            }
            return 0;
        }
        
        return 0;
    }

    int CTCPHelper::RecvInfo( int Socket, char* szInfo, int iLen )
    {
        char* pBuffer = szInfo;
        int	nBytesTransferred = 0;
        int	nBytesForTransfer = iLen;
        
        for( int i = 0, iRet = 0; i < nRetryTime; i++ )
        {
            iRet = RecvInfoRaw( Socket, pBuffer, nBytesForTransfer );
            if( iRet > 0 )
            {
                nBytesTransferred += iRet;
                nBytesForTransfer -= iRet;
                if( nBytesForTransfer > 0 )
                {
                    i = 0;
                    pBuffer += iRet;
                    continue;
                }
                return nBytesTransferred;
            }
            else if( iRet == -1 )
            {
                if( i < nRetryTime - 1 )
                    continue;
                else
                    return -1;
            }
            return 0;
        }
        
        return 0;
    }

 
    bool CTCPHelper::Close( int Socket )
    {
        shutdown( Socket, 2 );
        close( Socket );
        Socket = 0;
        return true;
    }
}
