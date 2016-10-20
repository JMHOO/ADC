import socket
import SocketServer
from time import ctime
import json, types,string
import threading
import datetime
#json:[{'operate': "" ,'key':"", 'value':""}]
#msg1 = [{'operate':"put", 'key':"1",'value':"12345"}]
#msg2 = [{'operate':"get", 'key':"1",'value':"null"}]
#msg3 = [{'operate':"delete", 'key':"null",'value':"null"}]
#
#jmsg1 = json.dumps(msg1)
#jmsg2 = json.dumps(msg2)
#jmsg3 = json.dumps(msg3)

fout = open('log.txt', 'w')

host = 'localhost'
port = 0;
operate = ""
#hostport 要输入
host = raw_input("please input host> ")
port = raw_input("please input port> ")
addr = (host,port)
#open socket
udpCliSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)##udp must use SOCK_DGRAM
#link to the server
#udpCliSock.connect((host, port))

idnum = 0;
#bag len
bufsiz = 1024;
while True:
    #data = raw_input('>' )
    idnum = idnum + 1;
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"the client's id is" + str(idnum) +' '+'\n')
    
    #wait for input
    operate = raw_input("please input operate> ")
    if operate == "delete":
        key = raw_input("please input key> ")
        value = "null"
        
    if operate == "get":
        key = raw_input("please input key> ")
        value = "null"
    if operate == "put":
        key = raw_input("please input key> ")
        value = raw_input("please input value> ")
    idarr = str(idnum);
    msg = [{'version':"1.0",'operate':operate, 'key':key,'value':value,'id':idarr}]
    jmsg = json.dumps(msg)
    data = jmsg;
    if not data:
        print("There is no valid data")
        fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" + '\n')
        break
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"the client's opearte is:" + operate + "(" + key + "," + value + ")" +' '+ '\n')
    
    #add the header
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
        print("There is no valid payload")
        fout.write(str(datetime.datetime.now())+' '+"The payload's type is incorrect" + '\n')
        break
    #send message
    udpCliSock.sendto(pkg_header,addr)
    udpCliSock.sendto(pkg_payload,addr)
    
    # ###########handle the response
    success = 0
    for i in range(1,3):
        udpCliSock.settimeout(3.0)
        response,addc = udpCliSock.recvfrom(bufsiz)
        success = 1;
        break
        udpCliSock.settimeout(None)
    #接下来跟tcp一样
    if success == 0:
        print("the client doesn't receive the response"+'\n')
        fout.write(str(datetime.datetime.now())+' '+"the client doesn't receive the response"+'\n')
        continue
    else:
        if not response:
            print("There is no valid response")
            fout.write(str(datetime.datetime.now())+' '+"The response's type is incorrect" +' '+  '\n')
            break
        #response ,earse the header
        if len(response) == PACK_HEADER_LENGTH:
            (version, type, length, resverse) = struct.unpack('!IIII',response)
            payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
            response = udpCliSock.recv(bufsiz)
            payload = struct.unpack(payload_fmt, response)
        elif len(response) > PACK_HEADER_LENGTH:
            (version, type, length, resverse) = struct.unpack('!IIII',response[:PACK_HEADER_LENGTH])
            payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
            payload = struct.unpack(payload_fmt, response[PACK_HEADER_LENGTH:])
            
        #write the log    
        recdata = json.dump(payload)
        
        
        for rec in recdata:
            if str(rec['result']['code']) == "0":
                recstr = "The value is " + str(rec['result']['value'])
                fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + "value:" + str(rec['result']['value']) + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] + " id:" + str(rec['id'])+' '+'\n')
            else:
                recstr = rec['result']['message']
                fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] + " id:" + str(rec['id'])+' '+'\n')
            print recstr
    
udpCliSock.close()
fout.close()
    