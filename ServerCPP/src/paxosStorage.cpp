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
    
    std::string Storage::Seek(const int64_t instanceId)
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
	        logger->Info("instanceId " + std::to_string(instanceId) + " is not exist while searching storage file.");
	        return "no data available";
	    } else {
	    	logger->Error("Storage Error: default storage file is not exist.");
	        std::cout << "file not exist" << std::endl;
	        return "";
	    }
	}

	void Storage::write(const int64_t instanceId, const int64_t proposalId, const std::string value)
	{
	    if(IsFileExist() == true) {
	        std::string data = std::to_string(instanceId) + " " + std::to_string(proposalId) + " " + value;
	        std::ofstream outfile (file_path, std::ios::out | std::ios::app);
	        outfile << data << std::endl;
	        outfile.close();
	        logger->Info("Storage Processing: Successfully write data {%s} to storage file.", data);
	    } else {
	        logger->Error("Storage Error: default storage file is not exist.");
	    }
	}
    
    std::string Storage::getFilePath() {
        return this->file_path;
    }

}
