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
        int tcpPort = 0;
        int rpcPort = 0;
        try
        {
            tcpPort = jrequest["tcpport"];
            rpcPort = jrequest["rpcport"];
        }
        catch(std::domain_error e)
        {
            std::string strPort = jrequest["tcpport"];
            tcpPort = std::stoi(strPort);
            strPort = jrequest["rpcport"];
            rpcPort = std::stoi(strPort);
        }
        int nNewID = srvMgr->RegisterServer(m_clientsocket, jrequest["address"], tcpPort, rpcPort);
        if( nNewID > 0 )
        {
            jresult["result"]["message"] = "register success.";
            jresult["result"]["value"] = std::to_string(nNewID);
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
        std::string strProtocol = jrequest["protocol"];
        std::transform(strProtocol.begin(),strProtocol.end(),strProtocol.begin(), ::tolower);

        bool bTCP = strProtocol == "tcp";
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
                si["port"] = bTCP ? srvList[i]->tcpport : srvList[i]->rpcport;
                
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

void jsonAgentPacket::BuildRegisterRequest(std::string strIP, int tcpport, int rpcport)
{
    json jReq;
    jReq["jsonagent"] = "1.0";
    jReq["operate"] = "register";
    jReq["address"] = strIP;
    jReq["tcpport"] = tcpport;
    jReq["rpcport"] = rpcport;
    
    m_json_result = jReq;
}
void jsonAgentPacket::BuildUnRegisterRequest()
{
    json jReq;
    jReq["jsonagent"] = "1.0";
    jReq["operate"] = "unregister";
    
    m_json_result = jReq;
}
void jsonAgentPacket::BuildGetServerListRequest(std::string serverType)
{
    json jReq;
    jReq["jsonagent"] = "1.0";
    jReq["operate"] = "getserverlist";
    jReq["protocol"] = serverType.c_str();
    
    m_json_result = jReq;
}

nlohmann::json jsonAgentPacket::GetServerList()
{
    return m_json_request["result"]["value"];
}

std::string jsonAgentPacket::GetValue()
{
    return m_json_request["result"]["value"];
}
