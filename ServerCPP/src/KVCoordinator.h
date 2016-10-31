
#ifndef __KVCoordinator_H_
#define __KVCoordinator_H_

class CKvCoordinator
{
public:
    static bool             Create();
    static void             Destory();
    static CKvCoordinator*  GetInstance();
    
private:
    static CKvCoordinator*  coordinator_instance;
};

#endif
