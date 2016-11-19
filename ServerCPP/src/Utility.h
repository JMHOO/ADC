/*===========================================================================
 File Description:
 Utility functions.
 ===========================================================================*/
#ifndef _ADCS__UTILITY_H_
#define _ADCS__UTILITY_H_


#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <stdarg.h>
#include "MTypes.h"

namespace ADCS
{
    namespace Utility
    {
        void						GetLocalTime( time_t time, time_t timezone, struct tm *tm_time );
        void						ltoa (          long value, char *str, int radix );
        void						ultoa( unsigned long value, char *str, int radix );
        void						SginalIgnore( int n, ...);
        bool						Daemon();
        
        ssize_t						ReadFile(  int fd, void * buf, size_t count );
        ssize_t						WriteFile( int fd, void * buf, size_t count );
        uint64_t                    GetCurrentTimeMS();
    }
}

#endif
