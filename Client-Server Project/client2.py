import socket
import time

HOST = '127.0.0.1'
PORT = 43595

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    data = s.recv(2001)
    print(data.decode())
    token = data.split()[0]
    port = data.split()[1]
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s2:
        s2.connect((HOST, int(port)))
        d = input()
        count = 0
        try:
            while True:
                t = 0
                s2.send(b"write")
                t = 1
                print(s2.recv(2001).decode())
                time.sleep(1)
                a = "{} {}".format(token.decode(), d)
                s2.send(a.encode('utf-8'))
                t = 2
                print(s2.recv(1000))
                count = count + 1
        except KeyboardInterrupt:
            if t == 0 or t == 2:
                pass
            else:
                time.sleep(1)
                a = "{} {}".format(token.decode(), d)
                s2.send(a.encode('utf-8'))
            time.sleep(2)
            s2.send(b"bye")
            print(s2.recv(102).decode())
            print(count)
