from jsonrpclib.SimpleJSONRPCServer import SimpleJSONRPCServer
from jsonrpclib import Server

host = 'localhost'
port = 1234
server = SimpleJSONRPCServer(( host, port, ))
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

server.register_function(add)
server.register_function(subtract)

server.serve_forever()