from jsonrpclib import Server
import json
import datetime
import argparse
import re
fout = open('logrpc.txt', 'w')

def handle(addr,operate,key,value,idnum):
        client = Server("http://"+addr)
#    while True:
        print("the client has linked to the server" + '\n')
        fout.write(str(datetime.datetime.now())+' '+"the client has linked to the server:" + addr +' '+ '\n')
        idnum = idnum + 1
        #write the log
        fout.write(str(datetime.datetime.now())+' '+"the client's id is " + str(idnum) +' '+'\n')
        #wait for input
        if operate == "DELETE":
    #        key = raw_input("please input key> ")
    #        value = "null"
            rec = client.Delete(key)
            print(rec['code'])
            fout.write(str(datetime.datetime.now())+' '+"the server's response is: "  + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
            print("the server's response is: "  + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
        if operate == "GET":
    #        key = raw_input("please input key> ")
    #        value = "null"
            rec = client.Get(key)
            print(rec)
            if rec['code'] == 0:
                fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + "value:" + str(rec['value']) + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
                print("the server's response is: " + "value:" + str(rec['value']) + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
            else:
                fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
                print("the server's response is: " + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
        if operate == "PUT":
    #        key = raw_input("please input key> ")
    #        value = raw_input("please input value> ")
            rec = client.Put(key,value)
            print(rec)
            fout.write(str(datetime.datetime.now())+' '+"the server's response is: "  + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
            print("the server's response is: " + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
         
    
    

parser = argparse.ArgumentParser(description='')
parser.add_argument('-p', '--port', dest='Port', metavar='15003', 
    help='Enter the server port')

parser.add_argument('-a', '--address', dest='Address',
    help='Enter server address', metavar='192.168.1.1')
args = parser.parse_args()
parser.print_help()


print args.Address
print args.Port
address = args.Address
port = args.Port
addr = str(address) +':'+str(port)
print(addr)
with open('operations.csv', 'r') as opFile:
    operations=[line.rstrip('\n') for line in opFile]
    print operations

#host = raw_input("please input host> ")
#port = raw_input("please input port> ")

# op format: PUT(1,45)
    
idnum = 0;   
for op in operations:
    idnum  =  idnum + 1;
    match = re.search(r"(\w+)\,(\w+)\,(\w+)", op)
    if match:
        operate = match.group(1)    # PUT
        key = match.group(2)    # 1
        value = match.group(3)    # 45
        print match.group(1)    # PUT
        print match.group(2)    # 1
        print match.group(3)    # 45
        fout.write(str(datetime.datetime.now())+' '+"the client's operate is: "  + operate + '(' + key + ',' + value + ')' +' '+'\n')
        handle(addr,operate,key,value,idnum)
#    else:
#        match = re.search(r"(\w+)\((\w+)\)", op)
#        operate = match.group(1)    # delete/get
#        key = match.group(2)    # 1
#        value =  "null"
#        handle(addr,operate,key,value,idnum)
#        fout.write(str(datetime.datetime.now())+' '+"the client's operate is: "  + operate + '(' + key  + ')' +' '+'\n')
#        print match.group(1)    # delete/get
#        print match.group(2)    # 1
        
fout.close()




