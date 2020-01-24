import socket

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
        while True:
            d = input()
            if d == 'write':
                s2.send(b"write")
                print(s2.recv(2001).decode())
                da = input()
                a = "{} {}".format(token.decode(), da)
                s2.send(a.encode('utf-8'))
            elif d == 'read':
                s2.send(b"read")
                print(s2.recv(1003).decode())
            else:
                s2.send(b"bye")
                for i in range(100000):
                    pass
                print(s2.recv(102).decode())

                break;
