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
    
    cout << "TCP server is started, 0.0.0.0:15001 LISTENING"<<endl;
    cout << "UDP Server is started, 0.0.0.0:15002 LISTENING"<<endl;
    cout << "--- type 'exit' to exit." << endl;
    
    // Server collection, start from port 15001, 15002....
    CServerApp server;
    server.Start(15001);
    
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
