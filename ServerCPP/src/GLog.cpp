#include "GLog.h"
#include <sys/time.h>
#include <string.h>

#ifdef _DEBUG
LogLevel GlobalLog::DEFAULT_LOG_LEVEL = LL_DEBUG;
#else
LogLevel GlobalLog::DEFAULT_LOG_LEVEL = LL_INFO;
#endif

GlobalLog::GlobalLog(const char* logName, LogLevel nlev)
{
    _level = nlev;
    memset(_logfile, 0, PATH_MAX);
    strcpy(_logName, logName);
}

GlobalLog::~GlobalLog(){  }

void GlobalLog::Debug(const char* formatstr, ...)
{
    va_list ap;
    va_start(ap,formatstr);
    log_to_file(LL_DEBUG, formatstr, ap);
    va_end(ap);
}

void GlobalLog::Trace(const char* formatstr, ...)
{
    va_list ap;
    va_start(ap,formatstr);
    log_to_file(LL_TRACE, formatstr, ap);
    va_end(ap);
}
void GlobalLog::Info(const char* formatstr, ...)
{
    va_list ap;
    va_start(ap,formatstr);
    log_to_file(LL_INFO, formatstr, ap);
    va_end(ap);
}

void GlobalLog::Warning(const char* formatstr, ...)
{
    va_list ap;
    va_start(ap,formatstr);
    log_to_file(LL_WARNNING, formatstr, ap);
    va_end(ap);
}

void GlobalLog::Error(const char* formatstr, ...)
{
    va_list ap;
    va_start(ap,formatstr);
    log_to_file(LL_ERROR, formatstr, ap);
    va_end(ap);
}

void GlobalLog::Fatal(const char* formatstr, ...)
{
    va_list ap;
    va_start(ap,formatstr);
    log_to_file(LL_FATAL, formatstr, ap);
    va_end(ap);
}

FILE* GlobalLog::open_log_file()
{
    time_t tmNow;
    time(&tmNow);
    struct tm tmLocal;
    localtime_r(&tmNow, &tmLocal);
    
    //Make log file and name
    snprintf(_logfile, PATH_MAX, "./%s.%02u%02u%02u.log", _logName, tmLocal.tm_year+1900, tmLocal.tm_mon, tmLocal.tm_mday);

    return fopen(_logfile, "a");
}

void GlobalLog::generate_log_header(LogLevel level, char* strHeadContent, size_t szHeadContentLenght)
{
    char date[40] = "";
    
    // get datetime
    time_t tDate = time(NULL);
    struct tm tmLocalNow;
    localtime_r(&tDate, &tmLocalNow);
    
    // get millisecond
    struct timeval tvTime;
    gettimeofday(&tvTime, NULL);

    snprintf(date, 40, "%02u-%02u-%02u %02u:%02u:%02u.%03u ", tmLocalNow.tm_year+1900, tmLocalNow.tm_mon, tmLocalNow.tm_mday,
             tmLocalNow.tm_hour, tmLocalNow.tm_min, tmLocalNow.tm_sec, tvTime.tv_usec/1000 );
    
    strcpy(strHeadContent,date);
    switch(level){
        case LL_DEBUG:
            strcat(strHeadContent,"[DEBUG]");
            break;
        case LL_INFO:
            strcat(strHeadContent,"[INFO]");
            break;
        case LL_TRACE:
            strcat(strHeadContent,"[TRACE]");
            break;
        case LL_WARNNING:
            strcat(strHeadContent,"[WARN]");
            break;
        case LL_ERROR:
            strcat(strHeadContent,"[ERROR]");
            break;
        case LL_FATAL:
            strcat(strHeadContent,"[CRIT]");
            break;
        default:
            break;
    }
    
    strcat(strHeadContent,"\t");
}

void GlobalLog::log_to_file(LogLevel level, const char* formatstr, va_list args)
{
    if( level < _level)
        return;
    
    FILE* logfilePtr = NULL;
    try{
        logfilePtr = open_log_file();
    }
    catch(...)
    {
        fprintf(stderr, "[%s,%d] open log file [%s] failed! \n", __FILE__, __LINE__, _logfile);
    }
    
    if( logfilePtr == NULL )
        return;
    
    const uint MAX_BUFFER_SIZE = 8192;
    char buffer[MAX_BUFFER_SIZE], header[PATH_MAX];
    memset(buffer, MAX_BUFFER_SIZE, 0);
    memset(header, PATH_MAX, 0);
    
    vsnprintf(buffer,MAX_BUFFER_SIZE,formatstr,args);
    strcat(buffer, "\n");

    generate_log_header(level, header, PATH_MAX);
    
    fwrite(header, sizeof(char), strlen(header), logfilePtr);
    fwrite(buffer, sizeof(char), strlen(buffer), logfilePtr);
    
    fflush(logfilePtr);
}
