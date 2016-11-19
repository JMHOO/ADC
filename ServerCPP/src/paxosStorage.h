//
//  paxosStorage.h
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef _ADCS__paxosStorage_H_
#define _ADCS__paxosStorage_H_

#include <string>

class ILog;

namespace Paxos
{
    class Storage
    {
    public:
        Storage(ILog* ptrLog);
        
        void Init();
        std::string Seek(const int Id);
        void write(const std:: string data);
        void setFilePath(const std::string newFilePath);
        std::string getFilePath();
        
        
    private:
        ILog* logger;
        
        std::string file_path;
        void CreateFile();
        bool IsFileExist();
    };
}

#endif /* paxosStorage_h */
