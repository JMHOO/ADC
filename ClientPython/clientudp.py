import socket
import SocketServer
from time import ctime
import json, types,string
import threading
#json:[{'operate': "" ,'key':"", 'value':""}]
msg1 = [{'operate':"put", 'key':"1",'value':"12345"}]
msg2 = [{'operate':"get", 'key':"1",'value':"null"}]
msg3 = [{'operate':"delete", 'key':"null",'value':"null"}]

jmsg1 = json.dumps(msg1)
jmsg2 = json.dumps(msg2)
jmsg3 = json.dumps(msg3)

host = 'localhost'
port = 21566
bufsiz = 1024
addr = (host,port)
#open socket
udpCliSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)##udp must use SOCK_DGRAM
#link to the server
#udpCliSock.connect((host, port))

while True:
    #data = raw_input('>' )
    data = jmsg1;
    if not data:
        print 'data type is wrong!'
        break
    udpCliSock.sendto(data,addr)
    
udpCliSock.close()
    