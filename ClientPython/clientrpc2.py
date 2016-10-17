from jsonrpclib import Server
host = 'localhost'
port = 1234
addr = (host,port)
client = Server('http://localhost:1234')
while True:
    a = 0;
    b = 0;
    a = raw_input("please input a>" )
    b = raw_input("please input b>" )
    print("the sum is")
    print(client.add(a,b))
    print("the diffence is") 
    print(client.subtract(a,b))