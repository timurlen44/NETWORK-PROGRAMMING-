/*  			UDP CLIENT                     */
#include <iostream>
#include <winsock2.h>
using namespace std;
#define SERVER "127.0.0.1" 
#define BUFLEN 1024  
#define PORT 54005  
char message[BUFLEN] = "Hi Server!";
char answer[BUFLEN] = {};
int main()
{
	system("title UDP Client");
	puts("UDP Client Begin.");
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0){printf("Failed. Error Code: %d", WSAGetLastError());return 1;}
    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {printf("socket() failed with error code: %d", WSAGetLastError());return 2;}
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;server.sin_port = htons(PORT);server.sin_addr.S_un.S_addr = inet_addr(SERVER);
    if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{printf("sendto() failed with error code: %d", WSAGetLastError());return 3;}
    int slen = sizeof(sockaddr_in);
    if (recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen) == SOCKET_ERROR){printf("recvfrom() failed with error code: %d", WSAGetLastError());exit(0);}
    printf("Received From Server: %s\n",answer);
    closesocket(client_socket);
    WSACleanup();
    system("pause");
}





