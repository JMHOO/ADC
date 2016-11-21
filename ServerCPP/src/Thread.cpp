//
//  Thread.cpp
//  ServerCPP
//
//  Created by Jiaming on 11/14/16.
//  Copyright © 2016 Jiaming. All rights reserved.
//

#include "Thread.h"
#include <errno.h>
#include <signal.h>

namespace ADCS{
    
    static void* __thread_run__(void* p)
    {
        Thread* thread = (Thread*)p;
        thread->Run();
        return 0;
    }
    
    Thread::Thread() : _thread(0)
    {}
    Thread::~Thread()
    {}
    
    void Thread::Start(bool bSystemScope)
    {
        if( bSystemScope )
        {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
            
            pthread_create(&_thread, &attr, __thread_run__, this);
            
            pthread_attr_destroy(&attr);
        }
        else
        {
            pthread_create(&_thread, 0, __thread_run__, this);
        }
    }

    void Thread::Join()
    {
        pthread_join(_thread, 0);
    }
    
    void Thread::Detach()
    {
        pthread_detach(_thread);
    }
    
    pthread_t Thread::GetPThread() const
    {
        return _thread;
    }
    
    void Thread::Kill(int sig)
    {
        pthread_kill(_thread, sig);
    }
    
    void Thread::Sleep(int ms) {
        timespec t;
        t.tv_sec = ms / 1000;
        t.tv_nsec = (ms % 1000) * 1000000;
        
        int ret = 0;
        do {
            ret = ::nanosleep(&t, &t);
        } while (ret == -1 && errno == EINTR);
    }
}
