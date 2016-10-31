#include "jsonAgentPackage.h"
#include "ServerManager.h"
#include <string>
#include <algorithm>
#include "ErrorCode.h"

using namespace nlohmann;
using namespace ADCS;

jsonAgentPacket::jsonAgentPacket()
{
    
}

jsonAgentPacket::jsonAgentPacket(const char* pData, unsigned int nDataLen, int clientSocket) : IPacket(pData, nDataLen, clientSocket)
{
    try
    {
        m_json_request = json::parse(m_data.data);
    }
    catch(std::invalid_argument arg)
    {
        printf("parser json error: %s", arg.std::exception::what());
        // paser json error
    }
    catch(...)
    { }
}

jsonAgentPacket::~jsonAgentPacket()
{
    
}

void jsonAgentPacket::Process()
{
    if( m_json_request.is_array())
    {
        for(size_t i = 0; i < m_json_request.size(); i++)
        {
            m_json_result.push_back(__process_one_operation__(m_json_request[i]));
        }
    }
    else if(m_json_request.is_object())
    {
        m_json_result = __process_one_operation__(m_json_request);
    }
}

json jsonAgentPacket::__process_one_operation__(json jrequest)
{
    json jresult;
    jresult["jsonagent"] = "1.0";
    jresult["result"]["code"] = "0";
    jresult["result"]["message"] = "success";
    
    // check the version of package
    if( jrequest["jsonagent"] != "1.0" )
    {
        jresult["result"]["code"] = "800";
        jresult["result"]["message"] = "Unsupported protocol";
    }
    
    CServerManager* srvMgr = CServerManager::GetInstance();
    if( !srvMgr)
    {
        jresult["result"]["code"] = "1024";
        jresult["result"]["message"] = "Internal Server Error";
        return jresult;
    }
    
    std::string agentOperator = jrequest["operate"];
    std::transform(agentOperator.begin(),agentOperator.end(),agentOperator.begin(), ::tolower);
    if( agentOperator == "register")
    {
        int iport = 0;
        try
        {
            iport = jrequest["port"];
        }
        catch(std::domain_error e)
        {
            std::string strPort = jrequest["port"];
            iport = std::stoi(strPort);
        }
        bool bRet = srvMgr->RegisterServer(m_clientsocket, jrequest["address"], iport);
        if( bRet )
        {
            jresult["result"]["message"] = "register success.";
        }
        else
        {
            jresult["result"]["code"] = std::to_string((int)901);
            jresult["result"]["message"] = "register server failed.";
        }
    }
    else if(agentOperator == "getserverlist")
    {
        std::string strValue = "";
        std::vector<PServerInfo> srvList = srvMgr->GetAliveServers();
        if( srvList.size() == 0 )
        {
            jresult["result"]["code"] = std::to_string((int)903);
            jresult["result"]["message"] = "No server alive!";
        }
        else
        {
            for(size_t i = 0; i < srvList.size(); i++)
            {
                json si;
                si["address"] = srvList[i]->serverAddr;
                si["port"] = srvList[i]->port;
                
                jresult["result"]["value"].push_back(si);
            }
            jresult["result"]["message"] = "get server list success";
        }
    }
    else if(agentOperator == "unregister")
    {
        bool bRet = srvMgr->UnregisterServer(m_clientsocket);
        if( bRet )
        {
            jresult["result"]["message"] = "Unregister Success";
        }
        else
        {
            jresult["result"]["code"] = std::to_string((int)902);
            jresult["result"]["message"] = "Server already removed.";
        }
    }
    else
    {
        jresult["result"]["code"] = "900";
        jresult["result"]["message"] = "Invalid Operator!";
    }
    
    return jresult;
}

bool jsonAgentPacket::IsValid() const
{
    if(!IPacket::IsValid()) return false;
    
    if( m_json_request.is_object())
    {
        if(m_json_request["jsonagent"] == "1.0")
            return true;
    }
    
    return false;
}

bool jsonAgentPacket::GetResult(char*& pStreamData, unsigned long& ulDataLen)
{
    std::string strResult = m_json_result.dump();
    
    ulDataLen = strResult.length();
    pStreamData = new char[ulDataLen];
    if(pStreamData == nullptr)
    {
        ulDataLen = 0;
        return false;
    }
    memcpy(pStreamData, strResult.c_str(), ulDataLen);
    
    return true;
}

void jsonAgentPacket::BuildRequest(std::string strOperate, std::string strIP, int port)
{
    json jReq;
    jReq["jsonagent"] = "1.0";
    jReq["operate"] = strOperate;
    jReq["address"] = strIP;
    jReq["port"] = port;
    
    m_json_result = jReq;
}

nlohmann::json jsonAgentPacket::GetServerList()
{
    return m_json_request["result"]["value"];
}
