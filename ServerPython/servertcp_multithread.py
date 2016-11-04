from socket import *
import SocketServer
import time
import json, types,string
import threading
import datetime
import argparse
import re
import struct
import binascii
from jsonrpclib.SimpleJSONRPCServer import SimpleJSONRPCServer
from jsonrpclib import Server
#json:{'operate': "" ,'key':"", 'value':""}#no[]means just a  json not json array
global dictop 
global dictkey 
global dictvalue
global dictdis
global dict
dict = {}#reserve key:value
dictdis ={}#reserve address:port  from the discovery
bufsiz = 1024
PACK_HEADER_LENGTH = 16
fout = open('logtcpserver.txt', 'w')

###############################################################################
def SrvPut(coorid,clientid,key,value):
    print "rpcSrvput"
    #msg = {"code":code, "value":value,"message":value}
    #dict[key] = value
    ##this should write SrvGo whether success
    global dictop 
    global dictkey 
    global dictvalue
    global dictdis
    dictop = "Put"
    dictkey = key
    dictvalue = value
    msg = {'code':0, 'message':"pre-put success"}
    jmsg = json.dumps(msg)
    #data = jmsg;
    #if not data:
    #    print("There is no valid data")
    #    fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"other server recieves the client's opearte is:" + "put" + "(" + key + "," + value + ")" +' '+ '\n')
    return msg
###############################################################################
def SrvDelete(coorid,clientid,key,value):
    global dictop 
    global dictkey 
    global dictvalue
    global dictdis
    print "rpcSrvdelete"
    dictop = "Delete"
    dictkey = key
    msg = {'code':0, 'message':"pre-delete success"}
#    jmsg = json.dumps(msg)
    #data = jmsg;
    #if not data:
    #    print("There is no valid data")
    #    fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"other server recieves the client's opearte is:" + "delete" + "(" + key +")" +' '+ '\n')
    
    return msg
###############################################################################
def SrvGo(coorid,clientid):
    global dictop 
    global dictkey 
    global dictvalue
    global dictdis
    global dict
    print "rpcSrvgo"
    if dict.has_key(dictkey):
        if dictop == 'Put':
            dict[dictop] = dictvalue
        elif dictop == 'Delete':
            del dict[dictkey]
        msg = {"code":0,"message":"success"}
    else:
        msg = {"code":1,"message":"unsuccess"}
    jmsg = json.dumps(msg)
    #data = jmsg;
    #if not data:
    #    print("There is no valid data")
    #    fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"other server recieves the client's opearte is:" + dictop + "(" + dictkey + ")" +' '+ '\n')
    return msg
###############################################################################
def Put(key,value):
    global dict
    print "rpcput"
    #msg = {"code":code, "value":value,"message":value}
    dict[key] = value
    msg = {"code":"0", "value":"0","message":"message is empty"}
    jmsg = json.dumps(msg)
    data = jmsg;
    if not data:
        print("There is no valid data")
        fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"other server recieves the client's opearte is:" + "put" + "(" + key + "," + value + ")" +' '+ '\n')
    return jmsg
###############################################################################
def Delete(key):
    global dict
    print "rpcdelete"
    if(dict.has_key(key)):
        del dict[key]
        msg = {"code":"0", "value":"0","message":"message is empty"}
    else:
        msg = {"code":"1", "value":"0","message":"server will explain"}
        print "there is no key in the dict"
    jmsg = json.dumps(msg)
    data = jmsg;
    if not data:
        print("There is no valid data")
        fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"other server recieves the client's opearte is:" + "delete" + "(" + key +")" +' '+ '\n')
    
    return jmsg
###############################################################################
def Get(key):
    global dict
    print "rpcget"
    if(dict.has_key(key)):
        msg = {"code":"0", "value":dict[key],"message":"message is empty"}
    else:
        msg = {"code":"1", "value":"0","message":"server will explain"}
        print"there is no key in the dict"
    jmsg = json.dumps(msg)
    data = jmsg;
    if not data:
        print("There is no valid data")
        fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"other server recieves the client's opearte is:" + "get" + "(" + key + ")" +' '+ '\n')
    return jmsg
###############################################################################
class ThreadJSONRPCServer(SocketServer.ThreadingMixIn,SimpleJSONRPCServer): 
    pass    
###############################################################################
class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler): #the function of tcp server and rpc client
    def handle(self):
        global dictdis
        global dict
        print '...connected from:', self.client_address
        #receive client message
        while True:
            cur_thread = threading.current_thread()
            #print(cur_thread.name)
            fout.write(str(datetime.datetime.now())+' '+"The server's threadname is " +' '+ str(cur_thread) + '\n')
            request = self.request.recv(bufsiz)
            payload
            if not request:
                print("There is no valid request")
                fout.write(str(datetime.datetime.now())+' '+"The request's type is incorrect" +' '+  '\n')
                break
            #response ,earse the header
            if len(request) == PACK_HEADER_LENGTH:
                (version, type, length, resverse) = struct.unpack('!IIII',request)
                payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
                request = self.request.recv(bufsiz)
                payload = struct.unpack(payload_fmt, request)
            elif len(request) > PACK_HEADER_LENGTH:
                (version, type, length, resverse) = struct.unpack('!IIII',request[:PACK_HEADER_LENGTH])
               # print  (version, type, length, resverse)        
                payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
                payload = struct.unpack(payload_fmt, request[PACK_HEADER_LENGTH:])
            #write the log
            #this payload may be payload[0]
            reqdata = payload
            req = json.loads(payload[0])
            
            req_key = req['key']
            req_value = req['value']
            req_operate = req['operate']
            req_id = req['id']
            value = "0"
            code = "0"
            fout.write(str(datetime.datetime.now())+' '+"The client's id is:  " + req_id + '\n')
            fout.write(str(datetime.datetime.now())+' '+"The client's operate is: " +' '+ req_operate + "(" + req_key + "," + req_value + ")" +' '+ '\n')
            if req_operate == "PUT":
                    print "put"
                    code = "0"
                    dict[req_key] = req_value
                    value  = req_value
            elif req_operate == "DELETE":
                    print "delete"
                    if(dict.has_key(req_key)):
                        value = dict[req_key]
                        del dict[req_key]
                        code = "0"
                    else:
                        code = "1"
                        print "there is no this key in the client"
            elif req_operate == "GET":
                    print "get"
                    if(dict.has_key(req_key)):
                        value = dict[req_key]
                        code = "0"
                    else:
                        code = "1"
                        print "there is no this key in the client"
            
            if code == "0":#this says we have succeed, we should put this message to rpcserver which mean other server
                message = "message is empty"
                fout.write(str(datetime.datetime.now())+' '+"The server's operate is: " +' '+ req_operate + "(" + req_key + "," + req_value + ")" +' '+ '\n')
            else:
                message = "server will explain"
                fout.write(str(datetime.datetime.now())+' '+"The server's operate is wrong " +' '+'\n')
                
            
            
            msg = {"jsonkv": "1.0","result": {"value": value, "code": code, "message": message},"id":req_id}
            jmsg = json.dumps(msg)
            data = jmsg;
            if not data:
                print("There is no valid data")
                fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
            #write the log
            #fout.write(str(datetime.datetime.now())+' '+"the client's opearte is:" + operate + "(" + key + "," + value + ")" +' '+ '\n')
            
            #add the header
            datalen = len(data)
            #print("the datalen is:"+str(datalen))
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
                break
            #send message
            fout.write(str(datetime.datetime.now())+' '+"The server has send the response successful" +' '+ '\n')
            self.request.sendall(pkg_header)
            self.request.sendall(pkg_payload)
            ##################################
            dictdiscopy = dictdis
            if code == "0":#this says we have succeed, we should put this message to rpcserver which mean other server
                message = "message is empty"
                for hostserver in dictdiscopy:
                    addserver = str(hostserver)+":"+str(dictdis[hostserver])
                    print(addserver)
                    try:
                        client = Server("http://"+addserver)#the addserver is in dictdis
                        if req_operate == "PUT":
                            rec  = client.Put(req_key,req_value)
                        elif req_operate == "DELETE":
                            rec = client.Delete(req_key)
                        elif req_operate == "GET":
                            rec = client.Get(req_key)
                        print "rpc's respond(other server's respond)" + rec
                    except:
                        print "cannot find other server"
            ##################################
######Reg###############################################################################
def handle(operate,hostip,porttcp,postrpc):  #handle linkdiscovery 
    global dictdis
    global dict
    msgRegSrv = {'jsonagent':"1.0",'operate':'register', 'address':hostip,'tcpport':porttcp,'rpcport':postrpc}
    msgGetSrvList = {'jsonagent':"1.0",'operate':'getserverlist', 'protocol':'rpc'}
    jmsg = json.dumps(msgRegSrv) if operate == 'register' else json.dumps(msgGetSrvList)
    data = jmsg;
    if not data:
        print("There is no valid data")
        fout.write(str(datetime.datetime.now())+' '+"The data's type is incorrect" +' '+ '\n')
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"the server's opearte is:" + operate + " " + str(hosttcp) + ":" + str(porttcp) +' '+ '\n')
    
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
    
    global payload
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
#        print  (version, type, length, resverse)        
        payload_fmt = '{0}s'.format(length-PACK_HEADER_LENGTH)
        payload = struct.unpack(payload_fmt, response[PACK_HEADER_LENGTH:])
        
    #handle the response
    recdata = payload
    #print(payload)
    rec = json.loads(payload[0])
    #fout.write(str(rec[0]['result'])+' '+'\n')
    if operate == 'getserverlist':
        if rec['result']['code'] == "0":
            #recstr = "The value is " + str(rec['result']['value']) + ' ' + rec['result']['message'] + '\n'
            recstr = "The server has get serverlist successfullly"
            fout.write(str(datetime.datetime.now())+' '+"the discovory's response is: " + "value:" + str(rec['result']['value']) + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] +' '+'\n')
        else:
            recstr = rec['result']['message']
            fout.write(str(datetime.datetime.now())+' '+"the discovory's response is: " + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] +' '+'\n')
        print recstr
        for i in range(0,len(rec['result']['value'])):
            dictdis[ rec['result']['value'][i]['address'] ]  =  rec['result']['value'][i]['port']
    elif operate == 'register':
        if rec['result']['code'] == "0":
            recstr = "The server has registered successfullly"
            fout.write(str(datetime.datetime.now())+' '+"the discovory's response is: " + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] +' '+'\n')
        else:
            recstr = rec['result']['message']
            fout.write(str(datetime.datetime.now())+' '+"the discovory's response is: " + " code:" + str(rec['result']['code']) + " message:" + rec['result']['message'] +' '+'\n')
        print recstr
##################################################################################### 
def resclient(hosttcp, porttcp):
    tcpServ = SocketServer.ThreadingTCPServer((hosttcp,porttcp),ThreadedTCPRequestHandler)
    tcpServ.serve_forever()
##################################################################################### 
def linkdiscovery(hostip,porttcp,portrpc):
    
    operate = 'register'
    handle(operate,hostip,porttcp,portrpc)
    while 1==1:
        time.sleep(10.0)
        operate = 'getserverlist'
        handle(operate,hostip,porttcp,portrpc)
###############################################################################
def AsRPCServer(hostrpc,portrpc):
    server = ThreadJSONRPCServer((hostrpc, portrpc))
    server.register_function(Put)
    server.register_function(Delete)
    server.register_function(SrvPut)
    server.register_function(SrvDelete)
    server.register_function(SrvGo)
    server.register_function(Get)
    server.serve_forever()        
#####################################################################################
if __name__=="__main__": 
    global dictop 
    global dictkey 
    global dictvalue
    global dictdis
    global dict
    dict = {}#reserve key:value
    dictdis ={}#reserve address:port  from the discovery
    dictop = ""
    dictkey = ""
    dictvalue = ""
    #we should handle the request of client
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-p', '--port', dest='Port', metavar='25000', 
        help='Enter the server port')
    parser.add_argument('-da', '--address', dest='Address',
        help='Enter server address', metavar='192.168.1.1')
    parser.add_argument('-ip', '--ip', dest='IP',
        help='Enter server address', metavar='192.168.1.1')
    args = parser.parse_args()
    #parser.print_help()
    #print args.Address
    #print args.Port
    if args.Port == None:
        parser.print_help()
        exit()  

    hosttcp = "0.0.0.0"
    porttcp = int(args.Port)#25000
    serverip = "0.0.0.0"
    fout = open('logtcpserver.txt', 'w')
    print " .... waiting for connection"
    
    
    #we should handle the link between server and the discovery
    hostdis = args.Address
    portdis = 15000
    serverip = "0.0.0.0"
    #addr = (serverip,port)
    addr = (hostdis,portdis)
    #open socket
    tcpCliSock = socket(AF_INET, SOCK_STREAM)#tcp must use  SOCK_STREAM
    #link to the server
    tcpCliSock.connect(addr)
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"the server has linked to the discovery (" + hostdis +"," + str(portdis) + ")"+' '+'\n')
   
   
    #we should done as a rpc server   
    hostrpc = '0.0.0.0'
    portrpc = int(args.Port) + 2
    
    
    #local host.ip
    hostip = args.IP
    #create thread object
    thread_list = list();
    thread_list.append(threading.Thread(target = resclient, name = "resclient", args = (hosttcp, porttcp,)))
    thread_list.append(threading.Thread(target = linkdiscovery, name = "linkdiscovery", args = (hostip,porttcp,portrpc,)))
    thread_list.append(threading.Thread(target = AsRPCServer, name = "AsRPCServer", args = (hostrpc, portrpc,)))
   
    #start all the thread in the list
    for thread in thread_list:  
        thread.start()  
    
    #the main thread wait for child thread to drop
    for thread in thread_list:  
        thread.join()  
fout.close()