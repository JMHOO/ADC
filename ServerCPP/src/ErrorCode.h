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
            KVStore = 8192,
            KVCoordinator = 16384,
            Other = 32768,
        };
        
        enum class General : unsigned int
        {
            OK = 0,
            Success = OK,
            Failed = 1,
            Overflowed = 2,
            
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
            
        enum class KVStore : unsigned int
        {
            Success = 0,
            KeyNotExist = (unsigned int)CodeBase::KVStore + 1,
            InvalidKey,
            ValueTooLong,
            Coor_MessageFailed,
            Coor_GoFailed,
            
        };
            
        const char* Explain(unsigned int code);
    }
}

#endif
