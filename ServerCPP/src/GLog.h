/*===========================================================================
 File Description:
 Log module.
 ===========================================================================*/
#ifndef _ADCS__LOG_H_
#define _ADCS__LOG_H_


#include <iostream>
#include <limits.h>

enum LogLevel
{
    LL_NONE		= 0XFF,
    LL_DEBUG	= 1,
    LL_TRACE	= 2,
    LL_INFO		= 3,
    LL_WARNNING	= 4,
    LL_ERROR	= 5,
    LL_FATAL	= 6
};

class ILog
{
public:
    virtual void Debug(const char* formatstr, ...) = 0;
    virtual void Trace(const char* formatstr, ...) = 0;
    virtual void Info(const char* formatstr, ...) = 0;
    virtual void Warning(const char* formatstr, ...) = 0;
    virtual void Error(const char* formatstr, ...) = 0;
    virtual void Fatal(const char* formatstr, ...) = 0;
    
    virtual ~ILog(){ };
};

class GlobalLog : public ILog
{
public:
    GlobalLog(const char* logName, LogLevel nlev);
    virtual ~GlobalLog();
    
    static LogLevel DEFAULT_LOG_LEVEL;
    
    virtual void Debug(const char* formatstr, ...);
    virtual void Trace(const char* formatstr, ...);
    virtual void Info(const char* formatstr, ...);
    virtual void Warning(const char* formatstr, ...);
    virtual void Error(const char* formatstr, ...);
    virtual void Fatal(const char* formatstr, ...);
    
private:
    FILE* open_log_file();
    void generate_log_header(LogLevel level, char* strHeadContent, size_t szHeadContentLenght);
    void log_to_file(LogLevel level, const char* formatstr, va_list args);
    
    char _logfile[PATH_MAX];
    char _logName[PATH_MAX];
    LogLevel _level;
    
};


#endif
