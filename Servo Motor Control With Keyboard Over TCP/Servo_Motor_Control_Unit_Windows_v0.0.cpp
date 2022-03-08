#include<winsock.h>
#include<iostream>
#include<conio.h>
#include<stdio.h>
#include<windows.h>

using namespace std;

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
int CheckKey(void) {
int key;
if (kbhit()) {
    key=getch();
    if (key == 224) {
        do {
            key=getch();
        } while(key==224);
        switch (key) {
            case 72:
                printf("up");
                return key;
            case 75:
                printf("left");
                break;
            case 77:
                printf("right");
                break;
            case 80:
                printf("down");
                return key;
        }
    }
    return key;//printf("%d\n",key);
}
}

int main()
{
cout<<"\t\t------ TCP CLIENT --------"<<endl;
cout<<endl;
//Local Variable
WSADATA       WinSockData;
int           iWsaStartup ;
int           iWsaCleanup ;

SOCKET        TCPClientSocket;//aslýnda int bir deðer typedef int typedef eski_isim yeni_isim;
int           iCloseSocket;

struct   sockaddr_in     TCPServerAdd;


int           iConnect;

int           iRecv;
char          RecvBuffer[512];
int           iRecvBuffer = 500;

int        iSend;
//char       SenderBuffer[512] = "Hello from Client!";
char       SenderBuffer[500];
int        iSenderBuffer = 500;

//STEP-1 WSASatrtUp Fun
iWsaStartup = WSAStartup(MAKEWORD(2,2),&WinSockData);
if ( iWsaStartup!= 0 )
{
cout<<"WSAStartUp Failed"<<endl;
}
cout<<"WSAStartUp Success"<<endl;

// STEP-2 Socket Creation
TCPClientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if (TCPClientSocket == INVALID_SOCKET)
{
cout<<"TCP Client Socket Creation Failed"<<endl;
cout<<"Error Code - "<<WSAGetLastError()<<endl;
}
cout<<"TCP Client Socket Creation Success"<<endl;

// STEP-3 Fill Server Structure
TCPServerAdd.sin_family = AF_INET;
TCPServerAdd.sin_addr.s_addr = inet_addr("192.168.100.5");
TCPServerAdd.sin_port = htons(8080);

// STEP-4 Connect Fun
iConnect = connect(//int connect (int sockfd, struct sockaddr *serv_addr, int addrlen)
                   TCPClientSocket,
                  (SOCKADDR*)&TCPServerAdd,
                  sizeof(TCPServerAdd));
if (iConnect == SOCKET_ERROR)
{
   cout<<"Connection Failed "<<endl;
   cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"Connection Success"<<endl;
// STEP-5 Send Data to Server
int key = 0;
while(1){
memset(SenderBuffer,'\0',500);
//printf("girecegin aci degeri = ");
//scanf("%s",SenderBuffer);
//puts("gonderilecek array = ");
if (kbhit()) {
			key = CheckKey();
            if(key == LEFT)
            	strcpy(SenderBuffer,"1");
            else if(key == RIGHT)
            	strcpy(SenderBuffer,"2");
			
			/*
            else if(CheckKey() == 80)
				puts("oldu"),strcpy(SenderBuffer,"2");*/
}
if(SenderBuffer[0] == '2' || SenderBuffer[0] == '1' ){
	puts(SenderBuffer);
iSend = send(TCPClientSocket,SenderBuffer,iSenderBuffer,0);
if (iSend == SOCKET_ERROR)
{
cout<<"Sending Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"Data Sending Success "<<endl;
}
Sleep(25);//100ms 
}

iCloseSocket = closesocket(TCPClientSocket);
if (iCloseSocket == SOCKET_ERROR)
{
cout<<"Closing Socket Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"Closing Socket Success"<<endl;
// STEP-8 WSA CleanUp Fun;
iWsaCleanup = WSACleanup();//program daha önceden çaðýrdýðý DLL ve bu uygulama için ayýrdýðý kaynaklarý geri iade ediyor
if (iWsaCleanup == SOCKET_ERROR)
{
cout<<"CleanUp Fun Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"CleanUp Fun Success"<<endl;
system("PAUSE");
return 0;

}

