import socket

 

msgFromClient       = "Hello Arduino".encode()


ServerAddressPort   = ("169.254.214.200", 54000)


client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)# AF_INET == IPv4 , SOCK_DGRAM == UDP
client.sendto(msgFromClient, ServerAddressPort)
receivedMessage = client.recvfrom(1024)
receivedMessage = "Received Message: "+str(receivedMessage[0].decode()) + "\nMessage from: " +str(receivedMessage[1])
print(receivedMessage)