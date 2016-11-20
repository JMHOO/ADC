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
        
        void Init();	//Init() check if default storage file is exist. Create file if not exist 
    	std::string Seek(const uint64_t instanceId);
    	void write(const uint64_t instanceId, const uint64_t proposalId, const std::string value);
        std::string getStorageFilePath();
        uint64_t GetMaxInstanceID();
        
        std::string getAccepterFilePath();
        bool WriteState(const uint64_t promisedProposalID, const int promisedNodeID, const uint64_t acceptedProposalID, const int acceptedNodeID, const std::string value);
        bool ReadState(uint64_t promisedProposalID, int& promisedNodeID, uint64_t& acceptedProposalID, int& acceptedNodeID, std::string& value);
        
    private:
        ILog* logger;
        
        const std::string storage_file = "paxosStorage.txt";
        const std::string accepter_file = "paxosAccepter.txt";
        void CreateFile(std::string file);
        bool IsFileExist(std::string file);
    };
}

#endif /* paxosStorage_h */
