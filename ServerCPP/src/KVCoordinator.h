
#ifndef __KVCoordinator_H_
#define __KVCoordinator_H_

#include <string>
#include <vector>
#include "ErrorCode.h"
#include "GLog.h"

using namespace std;

namespace ADCS
{
    
    typedef std::pair<string,int> ServerDesc;
    typedef std::vector<ServerDesc> ServerList;

    class CKvCoordinator
    {
    public:
        static bool             Create();
        static void             Destory();
        static CKvCoordinator*  GetInstance();
        
        void SetServerID(int sid) { m_serverID = sid; }
        void UpdateServerList(ServerList list);
        ServerList GetServerList();
        
        ErrorCode::KVStore SyncOperation(int nClientID, string strOperate, string key, string value);
        
        CKvCoordinator();
        ~CKvCoordinator();
    private:
        static CKvCoordinator*  coordinator_instance;
        ILog*                   m_logger;
        int                     m_serverID;
        ServerList              m_aliveSrvList;
    };

}
#endif
