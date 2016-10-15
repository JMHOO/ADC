from socket import *
import struct
import json
import binascii

PACK_HEADER_LENGTH = 16
#json:[{'operate': "" ,'key':"", 'value':""}]
msg1 = [{'operate':"put", 'key':"1",'value':"12345"}]
msg2 = [{'operate':"get", 'key':"1",'value':"null"}]
msg3 = [{'operate':"delete", 'key':"null",'value':"null"}]

jmsg1 = json.dumps(msg1)
jmsg2 = json.dumps(msg2)
jmsg3 = json.dumps(msg3)
host = '73.140.72.152'
port = 15001
bufsiz = 1024
#open socket
tcpCliSock = socket(AF_INET, SOCK_STREAM)
#link to the server
tcpCliSock.connect((host, port))
while True:
    #wait for input
    #data = raw_input('> ')
    data = jmsg2;
    # PackageHeader defination
    #struct _PackageHeader_
    #{
    #    unsigned int Version;			//-- Packet version.
    #    unsigned int Type;				//-- Packet type.
    #    unsigned int Length;			//-- The total length of whole packet.
    #    unsigned int Reserve;			//-- Reserved area. Decided by the packet type.
    #}
    datalen = len(data)
    packageformat='%ds' % datalen
    packagelen = PACK_HEADER_LENGTH + datalen
    headervalues = (100, 0, packagelen, 0)
    # ! means the bytes stream will use network order
    # or use htonl() instead. for example: packagelen = htonl(16+datalen)
    header_fmt = struct.Struct('!IIII')
    pkg_header = header_fmt.pack(*headervalues)
    pkg_payload = struct.pack(packageformat,data)
    if not pkg_payload:
        break
    #send message
    tcpCliSock.sendall(pkg_header)
    tcpCliSock.sendall(pkg_payload)
    #receive response
    response = tcpCliSock.recv(bufsiz)
    if not response:
        break
    if len(response) == PACK_HEADER_LENGTH:
        (version, type, length, resverse) = struct.unpack('!IIII',response)
        payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
        response = tcpCliSock.recv(bufsiz)
        payload = struct.unpack(payload_fmt, response)
    elif len(response) > PACK_HEADER_LENGTH:
        (version, type, length, resverse) = struct.unpack('!IIII',response[:PACK_HEADER_LENGTH])
        payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
        payload = struct.unpack(payload_fmt, response[PACK_HEADER_LENGTH:])
    print payload
tcpCliSock.close()
