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
#include "GLog.h"


int main(int argc, const char * argv[]) {


    std::cout << "Hello, World!\n";
    
    ILog* pLog = new GlobalLog("test", LL_DEBUG);
    pLog->Debug("hello my log");
    sleep(1);
    pLog->Info("important information %d", 11);
    pLog->Trace("test dddd");
    
    delete pLog;
    
    return 0;
}
