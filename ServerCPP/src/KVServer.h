#ifndef _ADCS__KVServer_H_
#define _ADCS__KVServer_H_

#include <string>
#include <map>
#include <pthread.h>
#include "ErrorCode.h"

typedef std::map<std::string, std::string> KVMap;

class CKVServer
{
public:
    static bool         Create();
    static void         Destory();
    static CKVServer*   GetInstance();
    
    CKVServer();
    ~CKVServer();
    
    ADCS::ErrorCode::KVStore  SetValue(std::string key, std::string value);
    ADCS::ErrorCode::KVStore  GetValue(std::string key, std::string &value);
    ADCS::ErrorCode::KVStore  Delete(std::string key);
    
private:
    pthread_mutex_t     setWriteLocker;
    KVMap               m_datamap;
    
    static CKVServer*   kvserver_instance;
};


#endif
