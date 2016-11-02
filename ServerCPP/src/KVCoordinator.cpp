
#include "KVCoordinator.h"
#include <jsonrpccpp/client/connectors/httpclient.h>
#include "CKvJSONRPCClient.h"
#include <algorithm>

namespace ADCS
{
    CKvCoordinator* CKvCoordinator::coordinator_instance = nullptr;

    bool CKvCoordinator::Create()
    {
        if( coordinator_instance == nullptr )
        {
            coordinator_instance = new CKvCoordinator();
            if( coordinator_instance == nullptr )
                return false;
        }
        return true;
    }

    void CKvCoordinator::Destory()
    {
        if(coordinator_instance)
        {
            delete coordinator_instance;
            coordinator_instance = nullptr;
        }
    }

    CKvCoordinator* CKvCoordinator::GetInstance()
    {
        return coordinator_instance;
    }

    CKvCoordinator::CKvCoordinator()
    {
        m_logger = new GlobalLog("coor", LL_DEBUG);
    }
    
    CKvCoordinator::~CKvCoordinator()
    {
        delete m_logger;
    }
    
    void CKvCoordinator::UpdateServerList(ServerList list)
    {
        this->m_aliveSrvList = list;
    }
    
    ErrorCode::KVStore CKvCoordinator::SyncOperation(int nClientID, string strOperate, string key, string value)
    {
        m_logger->Info("Coordinator: begin operation from client:%d -- %s,%s,%s", nClientID, strOperate.c_str(), key.c_str(), value.c_str());
        
        const int nRetryTimes = 3;
        // copy server list
        ServerList svl = m_aliveSrvList;
        
        ErrorCode::KVStore code = ErrorCode::KVStore::Success;
        
        std::transform(strOperate.begin(),strOperate.end(),strOperate.begin(), ::tolower);
        
        string strCoorID = std::to_string(m_serverID);
        string strClientID = std::to_string(nClientID);
        
        bool bSendSuccess = true;
        // 1-phrase
        for( size_t i = 0; i < svl.size(); i++ )
        {
            string strHttpURL = "http://" + svl[i].first + ":" + std::to_string(svl[i].second);
            
            m_logger->Info("Coordinator: commnicating with server: %s", strHttpURL.c_str());
            
            jsonrpc::HttpClient httpClient(strHttpURL);
            CKvRPCClient rpcClient(httpClient);
            
            for( int nCall = 0; nCall < nRetryTimes; nCall++ )
            {
                bSendSuccess = true;
                try {
                    if( strOperate == "put" )
                    {
                        rpcClient.SrvPut(strCoorID, strClientID, key, value);
                    }
                    else if( strOperate == "delete" )
                    {
                        rpcClient.SrvDelete(strCoorID, strClientID, key, value);
                    }
                } catch (jsonrpc::JsonRpcException e) {
                    m_logger->Error("Coordinator::SyncOperation error in 1-phrase: %s", e.what());
                    bSendSuccess = false;
                }
                
                if( bSendSuccess )break;
            }
            
            if( !bSendSuccess )
            {
                // sync to server failed, abort operation
                code = ErrorCode::KVStore::Coor_MessageFailed;
                break;
            }
            
        }
        
        // 2-phrase
        if( bSendSuccess )
        {
            m_logger->Info("Coordinator: all nodes acknowledge for 1-phrase, sending go...");
            bool bGoSuccess = true;
            
            for( size_t i = 0; i < svl.size(); i++ )
            {
                string strHttpURL = "http://" + svl[i].first + ":" + std::to_string(svl[i].second);
                
                jsonrpc::HttpClient httpClient(strHttpURL);
                CKvRPCClient rpcClient(httpClient);
                
                for( int nCall = 0; nCall < nRetryTimes; nCall++ )
                {
                    bGoSuccess = true;
                    try {
                        rpcClient.SrvGo(strCoorID, strClientID);
                    } catch (jsonrpc::JsonRpcException e) {
                        m_logger->Error("Coordinator::SyncOperation error in 2-phrase: %s", e.what());
                        bGoSuccess = false;
                    }
                    
                    if( bGoSuccess )break;
                }
                
                if( !bGoSuccess )
                {
                    // sync to server failed, abort operation
                    code = ErrorCode::KVStore::Coor_GoFailed;
                    break;
                }
                
            }
            
            if( bGoSuccess )
            {
                m_logger->Info("Coordinator: operation from client:%d was done!", nClientID);
            }
        }
        
        return code;
    }
}
