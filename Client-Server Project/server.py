import socket
import threading
import secrets

class clientThread (threading.Thread):
       def __init__(self, sockc, port, token):
          threading.Thread.__init__(self)
          self.sock = sockc
          self.port = port
          self.token = token
       def run(self):
          with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as cs:
              cs.bind((HOST, self.port))
              print("opened special socket!")
              cs.listen()
              ncs, c_addr = cs.accept()
              with ncs:
                  while True:
                      msg = ncs.recv(1024).decode()
                      print(msg)
                      if msg == "read":
                          print("in read!")
                          ab = readShared()
                          ab = 'Data = ' + ab
                          ncs.send(ab.encode('utf-8'))
                      if msg == "write":
                          ncs.send(b"Enter your data!")
                          c_tok_data = ncs.recv(1024).decode()
                          if c_tok_data.split()[0] == self.token:
                              data = ncs.send(b"Wait for data to be written!\n")
                              lockShared.acquire()
                              writeShared(c_tok_data.split()[1])
                              lockShared.release()
                              ncs.send(b"Data was written!")
                          else:
                              ncs.send(b"Token Incorrect!")
                      if msg == 'bye':
                          ncs.send(b"Okay, Bye!")
                          for i in range(100000):
                              pass
                          ncs.close()
                          break;

def generate_token():
    return secrets.token_hex(16)

def readShared():
    print(shared)
    return ', '.join(str(i) for i in shared)

def writeShared(data):
    shared.append(data)

### main:

lockShared = threading.Lock()
clients = []
shared = []

HOST = '127.0.0.1'
PORT = 43595

current_client_port = 7567

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(10)
    while True:
        newSock, addr = s.accept()

        with newSock:
            token = generate_token()
            t1 = clientThread(newSock, current_client_port, token)
            clients.append(t1)
            a = " {} {}".format(token, current_client_port)
            newSock.send(a.encode('utf-8'))
            current_client_port += 1
            t1.start()
