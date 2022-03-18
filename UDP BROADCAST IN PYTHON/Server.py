import socket
import time

Client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
Client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
Client.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
Client.settimeout(5)
message = b"BROADCAST"
print("Client begin.")
while True:
    Client.sendto(message, ('<broadcast>', 54000))
    msg, addr = Client.recvfrom(1024)
    print("Server IP Address: {}".format(addr[0]))
    print("Server Port: {}".format(addr[1]))
    print("Received Message From Server: {} ".format(msg))
    time.sleep(5)
