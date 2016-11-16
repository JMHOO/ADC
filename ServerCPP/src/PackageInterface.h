#ifndef _PackageInterface_H_
#define _PackageInterface_H_

#include "Network.h"

class IPacket
{
public:
    static IPacket* CreatePackage(const char* sdata, unsigned int uiDataLen, int clientSocket);

    //IPacket();
    IPacket();
    IPacket(const char* pData, unsigned int nDataLen, int clientSocket);
    IPacket(const IPacket& rhs);
    IPacket& operator = (const IPacket& rhs);
    
    virtual ~IPacket();
    virtual void Process(){ };
    virtual bool IsValid() const;
    virtual bool NeedResponse() const;
    
    bool ToBytes(const char*& pStreamData, unsigned long& ulDataLen);
    const ADCS::PACK_HEADER GetPackHead() const { return m_header; }
    
protected:
    virtual bool GetResult(char*& pStreamData, unsigned long& ulDataLen){ return false; }
    
protected:
    int m_clientsocket;
    ADCS::PACK_HEADER m_header;
    
    struct BufData{
        char* data;
        unsigned int length;
        BufData() : data(nullptr), length(0){}
    };
    BufData m_data;
    BufData m_bytesStream;
    
    void clear();
    void clear_bytesStream();
};

#endif
