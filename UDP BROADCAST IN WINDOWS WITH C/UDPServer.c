#include <stdio.h>
#include <winsock2.h>
#include <conio.h>

#define MYPORT 54000    // the port users will be connecting to

int main()
{
    WSADATA wsaData;
    struct sockaddr_in Recv_addr;
    struct sockaddr_in Sender_addr;
    int len = sizeof( struct sockaddr_in );
    char recvbuff[1024];
    int recvbufflen = 1024;
    Recv_addr.sin_family       = AF_INET;
    Recv_addr.sin_port         = htons( MYPORT );
    Recv_addr.sin_addr.s_addr  = INADDR_ANY;
    
    
    WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    SOCKET sock;
    sock = socket( AF_INET, SOCK_DGRAM, 0 );
    char broadcast = '1';

    if ( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof( broadcast ) ) < 0 )
    {
        printf("Error in setting Broadcast option");
        closesocket(sock);
        return 0;
    }

    
    if ( bind( sock,  (const struct sockaddr *)&Recv_addr, sizeof (Recv_addr) ) < 0 )
    {
        printf("Error in BINDING: %d",WSAGetLastError());
        getch();
        closesocket(sock);
        return 0;
    }
    


	while(1){
        printf("\nWaiting for message...\n");
        recvfrom( sock, recvbuff, recvbufflen, 0, (struct sockaddr*) &Sender_addr, &len );
         printf("Received Message is: %s",recvbuff);
    }

    closesocket(sock);
    WSACleanup();
}
