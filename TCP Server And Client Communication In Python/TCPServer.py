import socket
serv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serv.bind(("0.0.0.0", 82))
serv.listen(5)



print("Tcp Server")
while True:
    conn, addr = serv.accept()
    print('Client connected.')
    while True:
        data = conn.recv(4096)
        if not data: break
        print ('Receive From Client: {}'.format(data.decode()))
        conn.send("SERVER".encode('utf-8'))
    print('Connection is over.')
    conn.close()
