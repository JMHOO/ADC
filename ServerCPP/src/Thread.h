//
//  Thread.h
//  ServerCPP
//
//  Created by Jiaming on 11/14/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#ifndef Thread_h
#define Thread_h

#include <pthread.h>

namespace ADCS
{

    class Thread
    {
    public:
        Thread();
        virtual ~Thread();
        void Start(bool bSystemScope = false);
        void Join();
        void Detach();
        
        pthread_t GetPThread() const;
        
        void Kill(int sig);
        
        virtual void Run() = 0;
        
        static void Sleep(int ms);
        
    protected:
        pthread_t _thread;
    };

}

#endif /* Thread_h */
