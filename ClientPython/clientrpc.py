from jsonrpclib import Server
import json
import datetime
fout = open('log.txt', 'w')
addr = 'http://uw.umx.io:15003'
addr = raw_input("please input address>")
client = Server(addr)
idnum = 0;
while True:
    print("the client has linked to the server" + '\n')
    fout.write(str(datetime.datetime.now())+' '+"the client has linked to the server:" + addr +' '+ '\n')
    idnum = idnum + 1
    #write the log
    fout.write(str(datetime.datetime.now())+' '+"the client's id is " + str(idnum) +' '+'\n')
    #wait for input
    operate = raw_input("please input operate> ")
    if operate == "delete":
        key = raw_input("please input key> ")
        value = "null"
        rec = client.Delete(key)
        print(rec['code'])
        fout.write(str(datetime.datetime.now())+' '+"the server's response is: "  + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
        print("the server's response is: "  + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
    if operate == "get":
        key = raw_input("please input key> ")
        value = "null"
        rec = client.Get(key)
        print(rec)
        if rec['code'] == 0:
            fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + "value:" + str(rec['value']) + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
            print("the server's response is: " + "value:" + str(rec['value']) + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
        else:
            fout.write(str(datetime.datetime.now())+' '+"the server's response is: " + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
            print("the server's response is: " + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
    if operate == "put":
        key = raw_input("please input key> ")
        value = raw_input("please input value> ")
        rec = client.Put(key,value)
        print(rec)
        fout.write(str(datetime.datetime.now())+' '+"the server's response is: "  + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
        print("the server's response is: " + " code:" + str(rec['code']) + " message:" + rec['message'] +' '+'\n')
fout.close()   

