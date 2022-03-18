import socket

Server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
Server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
Server.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
Server.bind(("", 54000))
message = b"REPLY TO BROADCAST"
print("Server begin.")
while True:
    data, addr = Server.recvfrom(1024)
    Server.sendto(message, addr)
    print("Client IP Address: {}".format(addr[0]))
    print("Client Port: {}".format(addr[1]))
    print("Received Message From Client: {} ".format(data))
