//
//  paxosStorage.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "paxosStorage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "GLog.h"

namespace Paxos
{
    Storage::Storage(ILog* ptrLog) : logger(ptrLog)
    {
        
    }
    
    void Storage::Init()
    {
        if(IsFileExist() == false) {
            CreateFile();
        }
    }
    
    void Storage::CreateFile()
    {
        std::ofstream outfile (file_path);
        //outfile << "test test wei" << std::endl;
        outfile.close();
    }
    
    bool Storage:: IsFileExist()
    {
        std::ifstream ifile(file_path);
        return (bool)ifile;
    }
    
    std::string Storage::Seek(const uint64_t instanceId)
	{
	    if(IsFileExist() == true) {
	        std::ifstream storage_file(file_path);
	        for (std::string line; std::getline(storage_file, line);) {
	            std::vector<std::string> result;
	            std::istringstream iss(line);
	            while(iss) {
	                std::string colum;
	                iss >> colum;
	                result.push_back(colum);
	            }
	            if (std::to_string(instanceId) == result.front()) {
	                return line;
	            }

	        }
	        logger->Info("instanceId %lu is not exist while searching storage file.", instanceId);
	        return "no data available";
	    } else {
	    	logger->Error("Storage Error: default storage file is not exist.");
	        std::cout << "file not exist" << std::endl;
	        return "";
	    }
	}

	void Storage::write(const uint64_t instanceId, const uint64_t proposalId, const std::string value)
	{
	    if(IsFileExist() == true) {
	        std::string data = std::to_string(instanceId) + " " + std::to_string(proposalId) + " " + value;
	        std::ofstream outfile (file_path, std::ios::out | std::ios::app);
	        outfile << data << std::endl;
	        outfile.close();
	        logger->Info("Storage Processing: Successfully write data {%s} to storage file.", data.c_str());
	    } else {
	        logger->Error("Storage Error: default storage file is not exist.");
	    }
	}
    
    uint64_t Storage::GetMaxInstanceID()
    {
        return -1;
    }
    
    bool Storage::WriteState(const uint64_t promisedProposalID, const int promisedNodeID, const uint64_t acceptedProposalID, const int acceptedNodeID, const std::string value)
    {
        return true;
    }
    
    bool Storage::ReadState(uint64_t promisedProposalID, int& promisedNodeID, uint64_t& acceptedProposalID, int& acceptedNodeID, std::string& value)
    {
        return true;
    }
    
    std::string Storage::getFilePath() {
        return this->file_path;
    }

}
