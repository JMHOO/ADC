import socket
import SocketServer
from time import ctime
import json, types,string
import threading

host = ''
port = 21566
bufsiz = 1024

#open socket
udpSerSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)#udp must use SOCK_DGRAM
#bind to server
udpSerSock.bind((host, port))

while True:
    #wait client link
    print 'waiting for data...'
    data,addr = udpSerSock.recvfrom(bufsiz)
    if not data:
        print 'client has exited!'
        break
    print "got data from",addr
    jdata = json.loads(data)
    rec_operate = jdata[0]['operate']
    rec_key = jdata[0]['key']
    rec_value = jdata[0]['value']
    #we can do what we want to the this jdata
    response = [{'operate':rec_operate,'key':rec_key,'value':rec_value}]
    jresp = json.dumps(response)
    rec = rec_operate + "(" + rec_key +"," + rec_value + ")"
    print rec

udpSerSock.close()