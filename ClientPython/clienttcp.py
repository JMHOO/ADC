# -*- coding: utf-8 -*-
from socket import *
import json
#json:[{'operate': "" ,'key':"", 'value':""}]
msg1 = [{'operate':"put", 'key':"1",'value':"12345"}]
msg2 = [{'operate':"get", 'key':"1",'value':"null"}]
msg3 = [{'operate':"delete", 'key':"null",'value':"null"}]

jmsg1 = json.dumps(msg1)
jmsg2 = json.dumps(msg2)
jmsg3 = json.dumps(msg3)
host = 'localhost'
port = 21567
bufsiz = 1024
#open socket
tcpCliSock = socket(AF_INET, SOCK_STREAM)
#link to the server
tcpCliSock.connect((host, port))
while True:
    #wait for input
    #data = raw_input('> ')
    data = jmsg1;
    if not data:
        break
    #send message
    tcpCliSock.send(data)
    #receive response
    response = tcpCliSock.recv(bufsiz)
    if not response:
        break
    print response
tcpCliSock.close()

