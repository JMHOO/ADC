#ifndef _ADCS__ERRORCODE_H_
#define _ADCS__ERRORCODE_H_


namespace ADCS
{
    namespace ErrorCode
    {
        enum class CodeBase : unsigned int
        {
            General = 0,
            Network = 1024,
            ThreadPool = 2048,
            Helper = 4096,
            Other = 8192,
        };
        
        enum class General : unsigned int
        {
            OK = 0,
            SUCCESS = OK,
            FAILED = 1,
            OVERFLOW = 2,
            
        };

        enum class ThreadPool : unsigned int
        {
            NOINIT = (unsigned int)CodeBase::ThreadPool + 1,
            
        };
          
        enum class Network : unsigned int
        {
            Create = (unsigned int)CodeBase::Network + 1,
            Connect,
            Bind,
            Listen,
            InvalidIP,
            
        };
    }
}

#endif
