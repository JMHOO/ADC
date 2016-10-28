from socket import *
import SocketServer
from time import ctime
import json, types,string
import threading
import datetime
import argparse
import re
import struct
import binascii
#json:[{'operate': "" ,'key':"", 'value':""}]
dict = {}
bufsiz = 1024
PACK_HEADER_LENGTH = 16
fout = open('logtcpserver.txt', 'w')

class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler): 
    def handle(self):
        print '...connected from:', self.client_address
        #receive client message
        while True:
            cur_thread = threading.current_thread()
            #print(cur_thread.name)
            fout.write(str(datetime.datetime.now())+' '+"The server's threadname is " +' '+ str(cur_thread) + '\n')
            request = self.request.recv(bufsiz)
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
                payload = struct.unpack(payload_fmt, response[PACK_HEADER_LENGTH:])
            #write the log
            reqdata = payload[0]
            req = json.loads(reqdata)
            
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
            
            if code == "0":
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

if __name__=="__main__": 
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('-p', '--port', dest='Port', metavar='25000', 
        help='Enter the server port')
    
    args = parser.parse_args()
    #parser.print_help()
    #print args.Address
    #print args.Port
    if args.Port == None:
        parser.print_help()
        exit()  

    host = "0.0.0.0"
    port = int(args.Port)
    serverip = "0.0.0.0"
    fout = open('logtcpserver.txt', 'w')
    print " .... waiting for connection"
    #SocketServer.TCPServer.allow_reuse_address = True
tcpServ = SocketServer.ThreadingTCPServer((host,port),ThreadedTCPRequestHandler)
#    IP, PORT = tcpServ.server_address
#    # Start a thread with the server -- that thread will then start one
#    # more thread for each request
#    tcpServ_thread = threading.Thread(target=tcpServ.serve_forever)
#    # Exit the server thread when the main thread terminates
#    tcpServ_thread.daemon = True
#    tcpServ_thread.start()


tcpServ.serve_forever()
fout.close()














