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
        if(IsFileExist(storage_file) == false) 
        {
            CreateFile(storage_file);
        }
        if(IsFileExist(accepter_file) == false) {
            CreateFile(accepter_file );
        }
    }
    
    void Storage::CreateFile(std::string file)
    {
        std::ofstream outfile (file);
        outfile.close();
    }
    
    bool Storage:: IsFileExist(std::string file)
    {
        std::ifstream ifile(file);
        return (bool)ifile;
    }
    
    std::string Storage::Seek(const uint64_t instanceId)
	{
	    if(IsFileExist(storage_file) == true) {
	        std::ifstream storage(storage_file);
	        for (std::string line; std::getline(storage, line);) {
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
	    if(IsFileExist(storage_file) == true) {
	        std::string data = std::to_string(instanceId) + " " + std::to_string(proposalId) + " " + value;
	        std::ofstream outfile (storage_file, std::ios::out | std::ios::app);
	        outfile << data << std::endl;
	        outfile.close();
	        logger->Info("Storage Processing: Successfully write data {%s} to storage file.", data.c_str());
	    } else {
	        logger->Error("Storage Error: default storage file is not exist.");
	    }
	}
    
    uint64_t Storage::GetMaxInstanceID()
    {
        uint64_t id = 0;
        if(IsFileExist(storage_file) == true) {
            std::ifstream storage(storage_file);
            for (std::string line; std::getline(storage, line);) {
                std::vector<std::string> result;
                std::istringstream iss(line);
                while(iss) {
                    std::string colum;
                    iss >> colum;
                    result.push_back(colum);
                }

                uint64_t instanceID;
                std::istringstream stream(result.front());
                stream >> instanceID;
                if (instanceID > id)
                {
                    id = instanceID;
                }
            }
        }
        return id;
    }   
     
    bool Storage::WriteState(const uint64_t promisedProposalID, const int promisedNodeID, const uint64_t acceptedProposalID, const int acceptedNodeID, const std::string value)
    {
        if(IsFileExist(accepter_file) == true) {
            std::string data = std::to_string(promisedProposalID) + " " + std::to_string(promisedNodeID) + " " + std::to_string(acceptedProposalID) + " " + std::to_string(acceptedNodeID) + " " + value;
            std::ofstream outfile (accepter_file, std::ios::out | std::ios::app);
            outfile << data << std::endl;
            outfile.close();
            return true;
        } else {
            std::cout << "file not exist, please run Init() and create file " << std::endl;
            return false;
        }
    }

    bool Storage::ReadState(uint64_t promisedProposalID, int& promisedNodeID, uint64_t& acceptedProposalID, int& acceptedNodeID, std::string& value)
    {
        if(IsFileExist(accepter_file) == true) {
            std::ifstream accepter(accepter_file);
            for (std::string line; std::getline(accepter, line);) {
                std::vector<std::string> result;
                std::istringstream iss(line);
                std::string data[5];
                int i = 0;
                while(iss) {
                    std::string colum;
                    iss >> colum;
                    result.push_back(colum);
                    data[i] =colum;
                    i++;
                    if (i >= 5)
                    {
                        break;
                    }
                }
                if (std::to_string(promisedProposalID) == result.front()) {
                    promisedNodeID = atoi(data[1].c_str());
                    uint64_t accepted_proposal_id;
                    std::istringstream stream(data[2]);
                    stream >> accepted_proposal_id;
                    acceptedProposalID = accepted_proposal_id;
                    acceptedNodeID = atoi(data[3].c_str());
                    value = data[4];
                    return true;
                }

            }
            return false;
        } else {
            return false;
        }
    }
    

    std::string Storage::getStorageFilePath() 
    {
        return this->storage_file;
    }

    std::string Storage::getAccepterFilePath() 
    {
        return this->accepter_file;
    }
}
