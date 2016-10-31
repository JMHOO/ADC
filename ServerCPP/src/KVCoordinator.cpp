//
//  KVCoordinator.cpp
//  ServerCPP
//
//  Created by Jiaming on 10/31/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "KVCoordinator.h"



CKvCoordinator* CKvCoordinator::coordinator_instance = nullptr;

bool CKvCoordinator::Create()
{
    if( coordinator_instance == nullptr )
    {
        coordinator_instance = new CKvCoordinator();
        if( coordinator_instance == nullptr )
            return false;
    }
    return true;
}

void CKvCoordinator::Destory()
{
    if(coordinator_instance)
    {
        delete coordinator_instance;
        coordinator_instance = nullptr;
    }
}

CKvCoordinator* CKvCoordinator::GetInstance()
{
    return coordinator_instance;
}
