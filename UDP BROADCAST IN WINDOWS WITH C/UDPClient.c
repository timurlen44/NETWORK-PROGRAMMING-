#include <winsock2.h>
#include <conio.h>

#define MYPORT 54000    // the port users will be connecting to

int main()
{
    WSADATA wsaData;
    WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    SOCKET sock;
    sock = socket( AF_INET, SOCK_DGRAM, 0 );
    char broadcast = '1';
    if ( setsockopt( sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast) ) < 0 )
    {
        puts("Error in setting Broadcast option");
        closesocket(sock);
        return 0;
    }

    struct sockaddr_in Recv_addr;
    struct sockaddr_in Sender_addr;
    int len = sizeof( struct sockaddr_in );
    char sendMSG[] ="This is a broadcast from UDP Client";
    char recvbuff[50] = "";
    int recvbufflen = 50;
    Recv_addr.sin_family       = AF_INET;
    Recv_addr.sin_port         = htons( MYPORT );

    Recv_addr.sin_addr.s_addr = inet_addr( "127.0.0.1" );  // switch doesnt work
	//    Recv_addr.sin_addr.s_addr = inet_addr( "192.168.1.255" );  //wifi router works
    printf("sending message: %s",sendMSG);
    sendto( sock, sendMSG, (int) strlen(sendMSG)+1, 0,(const struct sockaddr*) &Recv_addr, sizeof( Recv_addr ) );

    closesocket( sock );
    WSACleanup();
}
