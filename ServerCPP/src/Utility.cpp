/*===========================================================================
 File Description:
 Utility functions.
 ===========================================================================*/
#include <new>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>								//-- for exit()
#include <fcntl.h>								//-- for open()
#include <sys/resource.h>						//-- for getrlimit()
#include <sys/time.h>
#include <inttypes.h>
#include "Utility.h"


/*===========================================================================
 DESCRIPTION:
 Calculate the local time & date.
 
 PARAMETERS:
	time [in] - The original time data return by time().
	timezone [in] - Time zone. e.g. The Beijing time zone is +8.
	tm_time [out] - The tm structure contain the local time & date.
 
 RETURN VALUE:
 None.
 ===========================================================================*/
void ADCS::Utility::GetLocalTime( time_t time, time_t timezone, struct tm *tm_time )
{
    const char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    unsigned  int uiLeapYearNumber;
    int iHourPerYear;
    
    //-- Calculate the time difference.
    time=time + timezone * 3600;		//-- Verify the time zone. 1 hour = 3600 seconds.
    
    if(time < 0)
    {
        time = 0;
    }
    //-- Calculate the seconds.
    tm_time->tm_sec = (int)(time % 60);
    time /= 60;
    //-- Calculate the minutes.
    tm_time->tm_min = (int)(time % 60);
    time /= 60;
    //-- Calculate the number of leap year in the past. There is a leap year every four years. 4 years = 1461 * 24 hours.
    uiLeapYearNumber = ((unsigned int)time / (1461L * 24L));
    //-- Calculate the years.
    tm_time->tm_year = (uiLeapYearNumber << 2) + 70;		//-- 1970
    //-- Calculate the hours remained in four years.
    time %= 1461L * 24L;
    //-- Verify the year affected by leap years, and calculate the hours remained in a year.
    for (;;)
    {
        //-- 1 year = 365 * 24 hours.
        iHourPerYear = 365 * 24;
        //-- Leap year judgement.
        if ((tm_time->tm_year & 3) == 0)
        {
            //-- It is leap year which contain a special day.
            iHourPerYear += 24;
        }
        if (time < iHourPerYear)
        {
            break;
        }
        tm_time->tm_year++;
        time -= iHourPerYear;
    }
    //-- The hours.
    tm_time->tm_hour = (int)(time % 24);
    //-- The days remaining in a year.
    time /= 24;
    //-- Assume it is a leap year.
    time += 1;
    //-- Adjust the error for leap year, and calculate the month and day.
    if ((tm_time->tm_year & 3) == 0)
    {
        if (time > 60)
        {
            time--;
        }
        else
        {
            if (time == 60)
            {
                tm_time->tm_mon = 1;
                tm_time->tm_mday = 29;
                return ;
            }
        }
    }
    //-- Calculate the months & days.
    for (tm_time->tm_mon = 0; Days[tm_time->tm_mon] < time;tm_time->tm_mon++)
    {
        time -= Days[tm_time->tm_mon];
    }
    
    tm_time->tm_mday = (int)(time);
}

/*===========================================================================
 Convert an unsigned long integer or a signed long integer to a string.
 
 PARAMETERS:
	val [in] - Number to be converted.
	buf [in, out] - The address of output buffer.
	radix [in] - Base of value.
 ===========================================================================*/
void ADCS::Utility::ltoa( long value, char *str, int radix )
{
    char *p = str;
    
    if( value < 0 )
    {
        *p++ = '-';
        value = -value;
    }
    
    ultoa( (unsigned long)value, p, radix );
}

void ADCS::Utility::ultoa( unsigned long value, char *str, int radix )
{
    char			*p = str;
    char			*p2 = str;
    char			temp;
    unsigned long	ulDigit;
    
    do
    {
        ulDigit = value % radix;
        value /= radix;
        
        if( ulDigit > 9 )
            *p++ = (char)( ulDigit - 10 + 'a' );
        else
            *p++ = (char)( ulDigit + '0' );
        
    } while (value > 0);
    
    *p-- = '\0';
    
    do
    {
        temp = *p;
        *p = *p2;
        *p2 = temp;
    } while( --p < ++p2 );
}

/*===========================================================================
 Ignore signals.
 ===========================================================================*/
void ADCS::Utility::SginalIgnore( int n, ...)
{
    try
    {
        int signum;
        
        va_list arg_ptr;
        va_start(arg_ptr, n);
        
        for ( int i = 0; i < n; i++ )
        {
            signum = va_arg(arg_ptr, int);
            signal( signum, SIG_IGN );
        }
        
        va_end(arg_ptr);
    }
    catch (...)
    {
    }
}

/*===========================================================================
 Let the caller process running in background. (Become to the daemon process.)
 The function Daemon(int, int) declared in "unistd.h" is Recommended.
 ===========================================================================*/
bool ADCS::Utility::Daemon()
{
    return 0;
    int					fd1, fd2;
    pid_t				pid;
    struct rlimit		rl;
    struct sigaction	sa;
    size_t				i;
    
    umask(0);
    
    if( getrlimit(RLIMIT_NOFILE, &rl) < 0 )
    {
        return false;
    }
    
    pid = fork();
    if( pid < 0 )
    {
        return false;
    }
    else if( pid != 0 ) /* parent */
    {
        exit(0);
    }
    setsid();
    
    //-- signal ------
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if( sigaction(SIGHUP, &sa, NULL) < 0 )
    {
        return false;
    }
    
    pid = fork();
    if( pid < 0 )
    {
        return false;
    }
    else if( pid != 0 ) /* parent */
    {
        exit(0);
    }
    
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);
    
    open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    
    return true;
}

ssize_t ADCS::Utility::ReadFile( int fd, void * buf, size_t count )
{
    ssize_t		ssizeReadBytes = 0;
    ssize_t		ssizeAllReadBytes = 0;
    
    while( count - (size_t)ssizeAllReadBytes )
    {
        ssizeReadBytes = read( fd, (char *)(buf) + ssizeAllReadBytes, count - (size_t)ssizeAllReadBytes );
        if( ssizeReadBytes < 0 )
        {
            if( errno == EINTR )			//-- This call was interrupted by the signal.
                continue;
            else if( errno == EAGAIN )		//-- No data can be read.
                return ssizeAllReadBytes;
            else
                return ssizeAllReadBytes;
        }
        else if( ssizeReadBytes == 0 )
            return ssizeAllReadBytes;
        ssizeAllReadBytes += ssizeReadBytes;
    }
    
    return ssizeAllReadBytes;
}

ssize_t ADCS::Utility::WriteFile( int fd, void * buf, size_t count )
{
    ssize_t		ssizeWriteBytes = 0;
    ssize_t		ssizeAllWriteBytes = 0;
    
    while( count - (size_t)ssizeAllWriteBytes )
    {
        ssizeWriteBytes = write( fd, (char *)(buf) + ssizeAllWriteBytes, count - (size_t)ssizeAllWriteBytes );
        if( ssizeWriteBytes < 0 )
        {
            if( errno == EINTR )
                continue;
            else if( errno == EAGAIN )
                return ssizeAllWriteBytes;
            else
                return ssizeAllWriteBytes;
        }
        else if( ssizeWriteBytes == 0 )
            return ssizeAllWriteBytes;
        ssizeAllWriteBytes += ssizeWriteBytes;
    }
    
    return ssizeAllWriteBytes;
}

uint64_t ADCS::Utility::GetCurrentTimeMS()
{
    uint64_t now;
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    
    now = tv.tv_sec;
    now *= 1000;
    now += tv.tv_usec / 1000;
    
    return now;
}
