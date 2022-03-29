/*  			UDP SERVER                    */
#include <iostream>
#include <winsock2.h>
using namespace std;
#define BUFLEN 1024
#define PORT 54005
char message[BUFLEN] = "Hi Client!";
char ReceivedData[BUFLEN] = "";
int main()
{
	system("title UDP Server");
	puts("UDP Server Begin.");
    sockaddr_in server, client;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0){printf("Failed. Error Code: %d", WSAGetLastError());exit(0);}  
    SOCKET server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){printf("Could not create socket: %d", WSAGetLastError());}
    server.sin_family = AF_INET;server.sin_addr.s_addr = INADDR_ANY;server.sin_port = htons(PORT);
    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR){printf("Bind failed with error code: %d", WSAGetLastError());exit(EXIT_FAILURE);}
    puts("Waiting for data...");
    int slen = sizeof(sockaddr_in);
    if (recvfrom(server_socket, ReceivedData, BUFLEN, 0, (sockaddr*)&client, &slen) == SOCKET_ERROR){printf("recvfrom() failed with error code: %d", WSAGetLastError());exit(0);}
    printf("Received packet from %s:%d\nReceived Data: %s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port),ReceivedData);
    if (sendto(server_socket, message, strlen(message), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{printf("sendto() failed with error code: %d", WSAGetLastError());return 3;}
    closesocket(server_socket);
    WSACleanup();
    system("pause");
}
