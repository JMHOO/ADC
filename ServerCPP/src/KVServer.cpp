#include <stdio.h>
#include "KVServer.h"

CKVServer* CKVServer::kvserver_instance = nullptr;

bool CKVServer::Create()
{
    if( kvserver_instance == nullptr )
    {
        kvserver_instance = new CKVServer();
        if( kvserver_instance == nullptr )
            return false;
    }
    return true;
}

void CKVServer::Destory()
{
    if(kvserver_instance)
    {
        delete kvserver_instance;
        kvserver_instance = nullptr;
    }
}

CKVServer* CKVServer::GetInstance()
{
    return kvserver_instance;
}

CKVServer::CKVServer()
{
    pthread_mutex_init( &setWriteLocker, NULL );
}

CKVServer::~CKVServer()
{
    pthread_mutex_destroy(&setWriteLocker);
}

ADCS::ErrorCode::KVStore CKVServer::SetValue(std::string key, std::string value)
{
    ADCS::ErrorCode::KVStore result = ADCS::ErrorCode::KVStore::Success;
    if( value.length() >= 1024 )
    {
        result = ADCS::ErrorCode::KVStore::ValueTooLong;
        return result;
    }
    
    KVMap::iterator it = m_datamap.find(key);
    pthread_mutex_lock(&setWriteLocker);
    if( it == m_datamap.end() )
    {
        m_datamap.insert(KVMap::value_type(key, value));
    }
    else
    {
        it->second = value;
    }
    m_datamap[key] = value;
    pthread_mutex_unlock(&setWriteLocker);
    
    return result;
}

ADCS::ErrorCode::KVStore CKVServer::GetValue(std::string key, std::string &value)
{
    ADCS::ErrorCode::KVStore result = ADCS::ErrorCode::KVStore::Success;
    KVMap::iterator it = m_datamap.find(key);
    if( it != m_datamap.end() )
    {
        value = it->second;
    }
    else
    {
        result = ADCS::ErrorCode::KVStore::KeyNotExist;
    }
    return result;
}

ADCS::ErrorCode::KVStore CKVServer::Delete(std::string key)
{
    ADCS::ErrorCode::KVStore result = ADCS::ErrorCode::KVStore::Success;
    KVMap::iterator it = m_datamap.find(key);
    pthread_mutex_lock(&setWriteLocker);
    if( it != m_datamap.end())
    {
        m_datamap.erase(it);
    }
    else
    {
        result = ADCS::ErrorCode::KVStore::KeyNotExist;
    }
    pthread_mutex_unlock(&setWriteLocker);
    return result;
}
