//
//  ErrorCode.cpp
//  ServerCPP
//
//  Created by Jiaming on 10/15/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include <stdio.h>
#include "ErrorCode.h"

namespace ADCS {
    namespace ErrorCode{

        const char* Explain(unsigned int code)
        {
            KVStore ecode = (KVStore)code;
            switch(ecode)
            {
                case KVStore::Success:  return "Sucess";
                case KVStore::InvalidKey: return "Invalid Key";
                case KVStore::KeyNotExist: return "Key does not exist.";
                case KVStore::ValueTooLong: return "Value must limited in 1024 bytes";
                default:
                    return "Unknown";
            }
        }
        
    }
}
