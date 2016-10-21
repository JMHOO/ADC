# -*- coding: utf-8 -*-
from socket import *
import json
import datetime
import argparse
import re
import struct
import binascii
from array import *


PACK_HEADER_LENGTH = 16

#json:[{'operate': "" ,'key':"", 'value':""}]
#msg1 = [{'operate':"put", 'key':"1",'value':"12345"}]
#msg2 = [{'operate':"get", 'key':"1",'value':"null"}]
#msg3 = [{'operate':"delete", 'key':"null",'value':"null"}]
#jmsg1 = json.dumps(msg1)
#jmsg2 = json.dumps(msg2)
#jmsg3 = json.dumps(msg3)
# build the log
fout = open('log.txt', 'w')


def handle(operate,key,value,idnum):
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"the client's id is " + str(idnum) +'\n')
    #wait for input
    idarr = str(idnum);
    msg = {'jsonkv':"1.0",'operate':operate, 'key':key,'value':value,'id':idarr}
    jmsg = json.dumps(msg)
    data = jmsg;
    if not data:
        print("There is no valid data")
        fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
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
        fout.write(str(datetime.datetime.now())+' '+"The payload's type is incorrect" +' '+ '\n')
    #send message
    tcpCliSock.sendall(pkg_header)
    tcpCliSock.sendall(pkg_payload)
    #receive response
    response = tcpCliSock.recv(bufsiz)
    if not response:
        print("There is no valid response")
        fout.write(str(datetime.datetime.now())+' '+"The response's type is incorrect" +' '+  '\n')
    #response ,earse the header
    if len(response) == PACK_HEADER_LENGTH:
        (version, type, length, resverse) = struct.unpack('!IIII',response)
        payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
        response = tcpCliSock.recv(bufsiz)
        payload = struct.unpack(payload_fmt, response)
    elif len(response) > PACK_HEADER_LENGTH:
        (version, type, length, resverse) = struct.unpack('!IIII',response[:PACK_HEADER_LENGTH])
        payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
        payload = struct.unpack(payload_fmt, response[PACK_HEADER_LENGTH:])
        
    #write the log
    recdata = payload[0]
    rec = json.loads(recdata)
    if rec['result']['code'] == "0":
        recstr = "The value is " + str(rec['result']['value'])
        fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + "value:" + str(rec['result']['value']) + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] + " id:" + str(rec['id'])+' '+'\n')
    else:
        recstr = rec['result']['message']
        fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] + " id:" + str(rec['id'])+' '+'\n')

#host = 'localhost'
#port = 0;
#operate = ""
##hostport 要输入
#host = raw_input("please input host> ")
#port = raw_input("please input port> ")
parser = argparse.ArgumentParser(description='')
parser.add_argument('-p', '--port', dest='Port', metavar='15001', 
    help='Enter the server port')

parser.add_argument('-a', '--address', dest='Address',
    help='Enter server address', metavar='192.168.1.1')
args = parser.parse_args()
parser.print_help()


print args.Address
print args.Port
host = args.Address
port = int(args.Port)
#open socket
tcpCliSock = socket(AF_INET, SOCK_STREAM)#tcp must use  SOCK_STREAM
#link to the server
tcpCliSock.connect((host, port))
#write the log
fout.write(str(datetime.datetime.now())+' '+"the client has linked to the server (" + host +"," + str(port) + ")"+' '+'\n')
#bag len
bufsiz = 1024;

with open('operations.txt', 'r') as opFile:
    operations=[line.rstrip('\n') for line in opFile]
    print operations



idnum = 0;
for op in operations:
    idnum  =  idnum + 1;
    match = re.search(r"(\w+)\((\w+)\,(\w+)\)", op)
    if match:
        operate = match.group(1)    # PUT
        key = match.group(2)    # 1
        value = match.group(3)    # 45
        print match.group(1)    # PUT
        print match.group(2)    # 1
        print match.group(3)    # 45
        fout.write(str(datetime.datetime.now())+' '+"the client's operate is: "  + operate + '(' + key + ',' + value + ')' +' '+'\n')
        handle(operate,key,value,idnum)
    else:
        match = re.search(r"(\w+)\((\w+)\)", op)
        operate = match.group(1)    # delete/get
        key = match.group(2)    # 1
        value =  "null"
        handle(operate,key,value,idnum)
        fout.write(str(datetime.datetime.now())+' '+"the client's operate is: "  + operate + '(' + key  + ')' +' '+'\n')
        print match.group(1)    # delete/get
        print match.group(2)    # 1
        
   
tcpCliSock.close()
fout.close()

