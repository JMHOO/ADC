
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include "TCPClient.h"
#include "UDPClient.h"

using namespace std;

struct _PackageHeader_
{
    unsigned int Version;			//-- Packet version.
    unsigned int Type;				//-- Packet type.
    unsigned int Length;			//-- The total length of whole packet.
    unsigned int Reserve;			//-- Reserved area. Decided by the packet type.
} __attribute__((aligned(4)));

typedef	_PackageHeader_	PACK_HEADER, *PtrPACK_HEADER;

int TestTCPClient( IClient &client, const char* ip, unsigned short port, long timeOut, int retryTime )
{
    char sInfo[] = "Hello, world!";
    
    int	len, recvedlen;
    char sBuffer[4096];
    memset(sBuffer, 0, 4096);
    PACK_HEADER *ptrPackage = (PtrPACK_HEADER)sBuffer;
    
    client.SetRetryTime( retryTime );
    client.SetTimeOut( timeOut );
    client.SetIP( ip );
    client.SetPort( port );
    
    ptrPackage->Length = (unsigned int)(strlen( sInfo ) + sizeof(PACK_HEADER));
    
     cout<<"connecting server -- "<<ip<<":"<<port<<" ..."<<endl;
    
    if( !client.Connect() )
    {
        cout<<"Connect to server failed."<<endl;
        return 1;
    }
    
    cout<<"Sending package to server, package size="<<ptrPackage->Length<<endl;
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
    cout<<"received data from server, data length="<<recvedlen<<endl;
    
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


int TestUDPClient(IClient &client, const char* ip, unsigned short port, long timeOut )
{
    char sInfo[] = "hello from UDP Client";
    
    //-- Helper: local variables.
    int len, recvedlen;
    char sBuffer[1024];
    PACK_HEADER *ptrPackage = (PACK_HEADER *)sBuffer;
    
    client.SetTimeOut( timeOut );
    client.SetIP( ip );
    client.SetPort( port );
    
    unsigned int uiPackageLength = (unsigned int)(strlen(sInfo) + sizeof(PACK_HEADER));
    
    cout<<"connecting server -- "<<ip<<":"<<port<<" ..."<<endl;
    
    //udp have no connection state, just prepare for session
    if( !client.Connect())
    {
        cout<<"Prepare UDP session failed."<<endl;
        return 1;
    }
    
    ptrPackage->Length = uiPackageLength;
    memcpy( sBuffer + sizeof(PACK_HEADER), sInfo, strlen(sInfo) );
    
    cout<<"Sending package to server, package size="<<uiPackageLength<<endl;
    // sending whole package directly
    len = client.SendInfo( sBuffer, (int)uiPackageLength );
    if( (int)uiPackageLength != len )
    {
        cout<<"Send package error. Expect to send "<<uiPackageLength<<" bytes, real send "<<len<<" bytes."<<endl;
        client.Close();
        return 1;
    }
    
    // receive package
    recvedlen = client.RecvInfo( sBuffer, 1024 );
    cout<<"received data from server, data length="<<recvedlen<<endl;
    
    if( recvedlen == 0 )		//-- The first.
    {
        cout<<"No response from server"<<endl;
        client.Close();
        return 0;
    }
    
    if( recvedlen < int(sizeof(PACK_HEADER)) )
    {
        cout<<"Recv package error. Package header is incomplete, close session"<<endl;
        client.Close();
        return 1;
    }
    
    if( recvedlen < (int)(ptrPackage->Length) )		//-- The third.
    {
        cout<<"Recv package error. Package data is incomplete. Need receive "<<ptrPackage->Length<<" bytes, real receive "<<recvedlen<<" bytes. close session."<<endl;
        client.Close();
        return 1;
    }
    
    sBuffer[recvedlen] = 0;
    
    cout<<"Server response: "<<sBuffer + sizeof(PACK_HEADER)<<endl;
    
    client.Close();
    
    return 0;
}

int main(int argc, const char * argv[]) {
    
    //cout<<"PACKAGE HEADER SIZE="<<sizeof(PACK_HEADER)<<endl;
    
    char ip[32] = {0};
    
    struct hostent *host;
    struct in_addr **addr_list;
    host = gethostbyname("uw.umx.io");
    addr_list = (struct in_addr **)host->h_addr_list;
    
    strcpy(ip, inet_ntoa(*addr_list[0]));
           
    cout << "Find server: uw.umx.io....." << ip << endl;
    
    strcpy(ip, "127.0.0.1");
    
    CTCPClient tcp_client;
    CUDPClient udp_client;
    
    
    unsigned short port = 15001;
    long	timeOut = 10;
    int		retryTime = 9;
    int     clientType = 1;
    
    cout<<"Please choose client type:"<<endl;
    cout<<"1 -- TCP Client"<<endl;
    cout<<"2 -- UDP Client"<<endl;
    
    cin>>clientType;
    cin.clear();
    cin.sync();
    
    if( clientType == 1)
        TestTCPClient(tcp_client, ip, port, timeOut, retryTime);
    else if(clientType==2)
        TestUDPClient(udp_client, ip, port+1, timeOut);
    
    return 0;
}
