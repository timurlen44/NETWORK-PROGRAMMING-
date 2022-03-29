import socket
msgFromClient       = "Hello UDP Server".encode('utf-8')
serverAddressPort   = ("127.0.0.1", 54000)

UDPClientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
UDPClientSocket.sendto(msgFromClient, serverAddressPort)
msgFromServer = UDPClientSocket.recvfrom(1024)
print("Message from Server {}".format(msgFromServer[0]))
UDPClientSocket.close()
