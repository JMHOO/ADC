
#include <iostream>
#include "TCPClient.h"

using namespace std;

struct _PackageHeader_
{
    unsigned int Version;			//-- Packet version.
    unsigned int Type;				//-- Packet type.
    unsigned int Length;			//-- The total length of whole packet.
    unsigned int Reserve;			//-- Reserved area. Decided by the packet type.
} __attribute__((aligned(4)));

typedef	_PackageHeader_	PACK_HEADER, *PtrPACK_HEADER;

int TestClient( IClient &client, const char* ip, unsigned short port, long timeOut, int retryTime )
{
    char sInfo[] = "Hello, world!";
    
    int	len, recvedlen;
    char sBuffer[4096];
    PACK_HEADER *ptrPackage = (PtrPACK_HEADER)sBuffer;
    
    client.SetRetryTime( retryTime );
    client.SetTimeOut( timeOut );
    client.SetIP( ip );
    client.SetPort( port );
    
    ptrPackage->Length = (unsigned int)htonl(strlen( sInfo ) + sizeof(PACK_HEADER));
    
    if( !client.Connect() )
    {
        cout<<"Connect to server failed."<<endl;
        return 1;
    }
    
    // send package header first
    len = client.SendInfo( sBuffer, (int)(sizeof(PACK_HEADER)) );
    if( (int)(sizeof(PACK_HEADER)) != len )
    {
        cout<<"Send package header error. Expect to send "<<sizeof(PACK_HEADER)<<" bytes, real send "<<len<<" bytes."<<endl;
        client.Close();
        return 1;
    }
    
    // send package payload
    len = client.SendInfo( sInfo, (int)strlen(sInfo) );
    if( (int)strlen(sInfo) != len )
    {
        cout<<"Send package payload error. Expect to send "<<strlen(sInfo)<<" bytes, real send "<<len<<" bytes."<<endl;
        client.Close();
        return 1;
    }
    
    // receive message from server
    recvedlen = client.RecvInfo( sBuffer, (int)(sizeof(PACK_HEADER)) );
    if( (int)(sizeof(PACK_HEADER)) > recvedlen )
    {
        cout<<"Recv package header error. Expect to receive "<<sizeof(PACK_HEADER)<<" bytes, real receive "<<len<<" bytes."<<endl;
        client.Close();
        return 1;
    }
    
    len = client.RecvInfo( sBuffer + recvedlen, (int)ntohl(ptrPackage->Length) - recvedlen );
    if( len != (int)ntohl(ptrPackage->Length) - recvedlen )
    {
        cout<<"Recv package payload error. Expect to receive "<<(int)ntohl(ptrPackage->Length) - recvedlen<<" bytes, real receive "<<len<<" bytes."<<endl;
        client.Close();
        return 1;
    }
    
    sBuffer[ntohl(ptrPackage->Length)] = 0;
    cout<<"Server response: "<<sBuffer + sizeof(PACK_HEADER)<<endl;
    
    client.Close();
    
    return 0;
}
int main(int argc, const char * argv[]) {
    
    CTCPClient tcp_client;
    
    char ip[] = "127.0.0.1";
    unsigned short port = 15001;
    long	timeOut = 10;
    int		retryTime = 9;
    
    cout<<"Please input the: IP and port"<<endl<<"Example: 127.0.0.1 15001"<<endl;
    cin>>ip>>port;
    cin.clear();
    cin.sync();
    
    TestClient(tcp_client, ip, port, timeOut, retryTime);
    
    return 0;
}
