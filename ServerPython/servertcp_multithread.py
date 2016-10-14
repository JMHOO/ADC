from socket import *
import SocketServer
from time import ctime
import json, types,string
import threading
import SocketServer
#json:[{'operate': "" ,'key':"", 'value':""}]

bufsiz = 1024
class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler): 
    def handle(self):
        print '...connected from:', self.client_address
        #receive client message
        while True:
            data = self.request.recv(bufsiz)
            if not data:
                break
            jdata = json.loads(data)
            print "Receive data from '%r'"% (data)
            print "Receive jdata from '%r'"% (jdata)
            rec_operate = jdata[0]['operate']
            rec_key = jdata[0]['key']
            rec_value = jdata[0]['value']
            
            cur_thread = threading.current_thread()  
            response = [{'thread':cur_thread.name,'operate':rec_operate,'key':rec_key,'value':rec_value}]
            
            jresp = json.dumps(response)
            self.request.sendall(jresp) 
            rec = cur_thread.name + "  "+rec_operate + "(" + rec_key +"," + rec_value + ")"
            print(rec)


if __name__=="__main__": 
    host = ''
    port = 21567
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















