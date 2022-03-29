import socket
localIP     = ""
localPort   = 54000
msgFromServer       = "Hello UDP Client".encode('utf-8')
UDPServerSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
UDPServerSocket.bind((localIP, localPort))
print("UDP server begin")
while(True):
    Data = UDPServerSocket.recvfrom(1024)
    print("Message from Client:{}".format(Data[0]))
    print("Client IP Address:{}".format(Data[1]))
    UDPServerSocket.sendto(msgFromServer, Data[1])
UDPServerSocket.close()
