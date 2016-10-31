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

typedef int (*TestFunc)(int, const char* [] );

int ShowHelp( int argc, const char* argv[] );
int Threadpool_Test( int argc, const char* argv[] );

class TestExecutor: public ADCS::IExecuteor
{
public:
    virtual bool Execute( void * lpvoid )
    {
        int f = *((int*)lpvoid);
        
        printf("== thread %03d before nano.\n", f);
        sleep(1);
        printf("== thread %03d after nano.\n", f);
        return true;
    }
    
    ~TestExecutor()
    {
    }
};

int id[1000];

int Threadpool_Test( int argc, const char* argv[] )
{
    ADCS::CThreadPool			pool;
    TestExecutor				executor;
    
    pool.Init(executor, 10, 20, 100);
    
    for(int i = 0; i < 200; i++)
    {
        id[i] = i;
        pool.WakeUp( &id[i] );
    }
    
    sleep(1);
    for(int i = 200; i < 400; i++)
    {
        id[i] = i;
        pool.WakeUp( &id[i] );
    }
    sleep(1);
    for(int i = 400; i < 700; i++)
    {
        id[i] = i;
        pool.WakeUp( &id[i] );
    }
    sleep(1);
    for(int i = 700; i < 1000; i++)
    {
        id[i] = i;
        pool.WakeUp( &id[i] );
    }
    
    
    unsigned int n = 0, t = 0, b = 0;
    unsigned int nTask = 0;
    for( int i = 0; i < 10; i++ )
    {
        sleep(1);
        pool.GetStatus( n, t, b, nTask );
        printf("--->>> normal: %d, burst: %d, working: %d, task: %d\n", n, t, b, nTask );
    }
    
    pool.Destory();
    
    printf("--->>>\n--->>>--->>> exit.\n");
    
    return 0;
}


struct CTestEntry
{
    const char* command;
    const char* desc;
    TestFunc TestEntry;
};

CTestEntry g_Entries[] = {
    { "help", (char*)"", ShowHelp },
    { "threadpool", "Thread Pool", Threadpool_Test }
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

int main(int argc, const char * argv[]) {
    
    cmdline::parser cmdParser;
    
    cmdParser.add<int>("port", 'p', "Base port number, should be ranged in 1000 and 65535, automatically increased by different kinds of server instance. For example:Base port number = 15000, then 15000 will assigen to TCP Server, 15001 for UDP Server and 15002 for RPC Server", true, 15001, cmdline::range(1000, 65535));
    cmdParser.add<std::string>("mode", 'm', "Config server mode, one of 'agent | regular', when config as agent server, the --port parameter will be ignored.", true, "regular");
    cmdParser.add<std::string>("address",'i', "Server address which will send to Agent server, the address should be connectable.", false, "127.0.0.1");
    cmdParser.add<std::string>("agentserver", 'a', "Agent server address, default=uw.umx.io", false, "uw.umx.io");
    
    cmdParser.parse_check(argc, argv);
    
    int nPort = cmdParser.get<int>("port");
    std::string sMode = cmdParser.get<string>("mode");
    std::string sAddr = cmdParser.get<string>("address");
    std::string sAgentSrvAddr = cmdParser.get<string>("agentserver");

    //ILog* pLog = new GlobalLog("test", LL_DEBUG);
    //pLog->Debug("hello my log");
    //sleep(1);
    //pLog->Info("important information %d", 11);
    //pLog->Trace("test dddd");

    
    //printf("begin test threadpool---->>>>>>");
    //TestThreadpool();
    
    //delete pLog;
    
    
    //cout<<"Enter port for service:"<<endl<<endl;
    
    string strPrefix = "ADC Server >>";
    string command;
    CTestEntry *ptr;
    bool find = false;
    
    //cout << "TCP server is started, 0.0.0.0:15001 LISTENING"<<endl;
    //cout << "UDP Server is started, 0.0.0.0:15002 LISTENING"<<endl;
    //cout << "RPC Server is started, 0.0.0.0:15003 LISTENING"<<endl;
    cout << "--- type 'exit' to exit." << endl;
    
    // Server collection, start from port 15001, 15002....
    CServerApp server;
    server.Start(nPort, sMode, sAddr, sAgentSrvAddr);
    
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
    
    //server.Stop();
    
    /*
    // for test only, sleep 10 minutes
    struct timespec		ts;
    ts.tv_sec = 10 * 60;	//-- 3 minutes.
    ts.tv_nsec = 0;
    nanosleep( &ts, NULL );
    */
    return 0;
}
