//
//  main.cpp
//  ADCServer
//
//  Created by Jiaming on 10/9/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "SrvApp.h"
#include "cmdline.h"

using namespace std;

CServerApp g_servers;
typedef int (*TestFunc)(int, const char* [] );
int ShowHelp( int argc, const char* argv[] );
int ListAllAliveServer(int argc, const char * argv[]);

struct CTestEntry
{
    const char* command;
    const char* desc;
    TestFunc TestEntry;
};

CTestEntry g_Entries[] = {
    { "help", (char*)"", ShowHelp },
    { "listserver", "List all online server", ListAllAliveServer }
};

const int g_EntriesNumber = sizeof(g_Entries)/sizeof(CTestEntry);
int ShowHelp( int argc, const char * argv[] )
{
    CTestEntry *ptr;
    
    cout<<"Command: help"<<endl<<"  -- Show the list."<<endl;
    cout<<"Command: exit"<<endl<<"  -- Exit the process."<<endl;
    
    for( int i = 1; i < g_EntriesNumber; i++ )
    {
        ptr = &(g_Entries[i]);
        
        cout<<"Command: "<<ptr->command<<endl;
        cout<<"  -- Test Unit: "<<ptr->desc<<endl;
    }
    
    return 0;
}
    
int ListAllAliveServer(int argc, const char * argv[])
{
    g_servers.ListAllServer();
    return 0;
}

int main(int argc, const char * argv[]) {
    
    cmdline::parser cmdParser;
    
    cmdParser.add<std::string>("mode", 'm', "KV Server or discovery server, one of 'discovery | kvserver', when config as discovery server, the --port parameter will be ignored.", true, "regular");
    cmdParser.add<int>("port", 'p', "Base port number for KV Server, should be ranged in 1000 and 65535, automatically increased by different kinds of server instance. For example:Base port number = 15000, then 15000 will assigen to TCP Server, 15001 for UDP Server and 15002 for RPC Server", false, 15001, cmdline::range(1000, 65535));
    cmdParser.add<std::string>("external",'e', "KV Server's external IP address or domain which will send to discovery server, the address then will be connected by client.", false, "127.0.0.1");
    cmdParser.add<std::string>("discovery", 'd', "Discovery server address, default=uw.umx.io", false, "uw.umx.io");
    
    cmdParser.parse_check(argc, argv);
    
    int nPort = cmdParser.get<int>("port");
    std::string sMode = cmdParser.get<string>("mode");
    std::string sExternalIP = cmdParser.get<string>("external");
    std::string sAgentSrvAddr = cmdParser.get<string>("discovery");
    
    if( sMode == "kvserver" && sExternalIP == "127.0.0.1" )
    {
        cout << "In KV Server mode, --external parameter must be specified, please see the usage." << endl;
        return 0;
    }
    
    string strPrefix = "KV Server >>";
    if( sMode == "discovery")
        strPrefix = "Discovery Server >>";
    string command;
    CTestEntry *ptr;
    bool find = false;
    
    cout << "--- type 'exit' to exit." << endl;
    
    // Server collection: TCP Server, UDP Server, RPC Server and Discovery Server
    if( !g_servers.Start(nPort, sMode, sExternalIP, sAgentSrvAddr) )
    {
        cout << "Failed to start all server, exit." << endl;
        return 0;
    }
    
    while(1)
    {
        cout<<strPrefix;
        cin>>command;
        cin.clear();
        cin.sync();
        
        ptr = NULL;
        find = false;

        if( command == "exit" )
            break;
        
        for( int i = 0; i < g_EntriesNumber; i++ )
        {
            ptr = &(g_Entries[i]);
            if( command == ptr->command )
            {
                find = true;
                break;
            }
        }
        
        if( find )
        {
            ptr->TestEntry( argc, argv );
        }
        else
        {
            cout<<"Bad Command! Please Retry!"<<endl;
        }
        
        cout<<endl;
        
    }
    
    g_servers.Stop();
    
    return 0;
}
