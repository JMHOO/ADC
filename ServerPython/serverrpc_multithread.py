from jsonrpclib.SimpleJSONRPCServer import SimpleJSONRPCServer
from jsonrpclib import Server
import SocketServer
host = 'localhost'
port = 1234

# A well defined API
def add(x, y):
    x = int(x)
    y = int(y)
    res = 0;
    rec = x + y
    return rec

def subtract(x, y):
    x = int(x)
    y = int(y)
    res = 0;
    rec = x - y
    return rec
    
class ThreadJSONRPCServer(SocketServer.ThreadingMixIn,SimpleJSONRPCServer): 
    pass

server = ThreadJSONRPCServer(('', port))
server.register_function(add)
server.register_function(subtract)

server.serve_forever()