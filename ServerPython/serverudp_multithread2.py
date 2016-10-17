from socket import *
import threading
import json
host = ''
port = 21566
bufsiz = 1024
class Server(threading.Thread) :

  def __init__(self) :
    threading.Thread.__init__(self)
    self.ADDR = (host ,port)
    self.sock = socket(AF_INET ,SOCK_DGRAM)
    self.sock.bind(self.ADDR)
    self.thStop = False

  def __del__(self) :
    self.sock.close()

  def transMsg(self) :
    (data , curAddr) = self.sock.recvfrom(bufsiz)
    jdata = json.loads(data)
    #print "Receive data from '%r'"% (data)
    #print "Receive jdata from '%r'"% (jdata)
    rec_operate = jdata[0]['operate']
    rec_key = jdata[0]['key']
    rec_value = jdata[0]['value']
            
    cur_thread = threading.current_thread()  
    response = [{'thread':cur_thread.name,'operate':rec_operate,'key':rec_key,'value':rec_value}]
    jresp = json.dumps(response) 
    rec = rec_operate + "(" + rec_key +"," + rec_value + ")"
    print(rec)

  def run(self) :
    while not self.thStop :
      self.transMsg()

  def stop(self) :
    thStop = True


def main() :
  ser = Server()
  print " .... waiting for connection"
  ser.start()
  
if __name__ == '__main__' :
  main()