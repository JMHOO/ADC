#ifndef _ACDS__Network_H_
#define _ACDS__Network_H_


namespace ADCS
{
    namespace Events
    {
        const int	EV_Connect = 0;
        const int	EV_Send = 1;
        const int	EV_Recv = 2;
        const int	EV_Close = 3;
        const int	EV_Exit = 4;
        const int	EV_RecvError = 5;
        const int	EV_SendError = 6;
        const int	EV_ThreadPoolError = 20;
        const int	EV_UnknownError = 99;
    }
    
    enum class ServerStatus { Uninit = 100, Inited, Running, Exiting};
}


#endif /* Network_h */
