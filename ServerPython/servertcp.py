from socket import *
from time import ctime
import json
#json:[{'operate': "" ,'key':"", 'value':""}]
host = ''
port = 21567
bufsiz = 1024
 
#open socket
tcpSerSock = socket(AF_INET, SOCK_STREAM)
#bind to server
tcpSerSock.bind((host, port))
#start listening
tcpSerSock.listen(5)
 
while True:
    #wait client link
    print 'waiting for connection...'
    tcpCliSock, adrr = tcpSerSock.accept()
    #print '...connected from:', addr
    while True:
        #receive client message
        data = tcpCliSock.recv(bufsiz)
        if not data:
            break
        jdata = json.loads(data)
        print "Receive data from '%r'"% (data)
        print "Receive jdata from '%r'"% (jdata)
        rec_operate = jdata[0]['operate']
        rec_key = jdata[0]['key']
        rec_value = jdata[0]['value']
        #send message to client
        #we can do what we want to the this jdata
        response = [{'operate':rec_operate,'key':rec_key,'value':rec_value}]
        jresp = json.dumps(response)
        rec = rec_operate + "(" + rec_key +"," + rec_value + ")"
        tcpCliSock.send('[%s] %s' %("You send:", rec))
    tcpCliSock.close()

tcpSerSock.close()
