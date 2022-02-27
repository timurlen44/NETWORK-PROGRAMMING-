#include<winsock.h>
#include<iostream>
#include<windows.h>
#include<string.h>
#define _OPEN_SYS_ITOA_EXT
#define PORT 54000
#define MAX_SIZE 1024
const char *ServerIP = "127.0.0.1";
using namespace std;
int64_t FileSize(const char* name);
int DosyaGonder(const char* music_name,char* SenderBuffer,int SenderBufferSize,SOCKET TCPClientSocket);
void ana_menu();
char music_name[50];
char file_size[20];
int main()
{
cout<<"\t\t------ TCP CLIENT --------"<<endl;
cout<<endl;
WSADATA WinSockData;
int iWsaStartup ;
int iWsaCleanup ;
SOCKET TCPClientSocket;
int iCloseSocket;
struct sockaddr_in TCPServerAdd;
int iConnect;
int iRecv;
char RecvBuffer[MAX_SIZE];
int iRecvBuffer = MAX_SIZE;
int iSend;
char SenderBuffer[MAX_SIZE];
int iSenderBuffer = MAX_SIZE;
iWsaStartup = WSAStartup(MAKEWORD(2,2),&WinSockData);
int64_t dosya_boyutu = 0;
if ( iWsaStartup!= 0 )
{
cout<<"WSAStartUp Failed"<<endl;
}

TCPClientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if (TCPClientSocket == INVALID_SOCKET)
{
cout<<"TCP Client Socket Creation Failed"<<endl;
cout<<"Error Code - "<<WSAGetLastError()<<endl;
}

TCPServerAdd.sin_family = AF_INET;
TCPServerAdd.sin_addr.s_addr = inet_addr(ServerIP);
TCPServerAdd.sin_port = htons(PORT);
iConnect = connect(
                   TCPClientSocket,
                  (SOCKADDR*)&TCPServerAdd,
                  sizeof(TCPServerAdd));
if (iConnect == SOCKET_ERROR)
{
   cout<<"Connection Failed "<<endl;
   cout<<"Error No-> "<<WSAGetLastError()<<endl;
   exit(1);
}
printf("Gonderilecek dosyanin ismini giriniz: ");
char dosyanin_isimi[100];
scanf("%s",dosyanin_isimi);

    int err_no = DosyaGonder(dosyanin_isimi,SenderBuffer,1024,TCPClientSocket);
    if(err_no == -1){
        cout<<"Dosya mevcut degil!"<<endl;
    }
    else if(err_no == -2){
        cout<<"Error No-> "<<WSAGetLastError<<endl;
        exit(1);
    }
    else if(err_no == -3){
        cout<<"Sunucudan onay mesaji gelmedi"<<endl;
    }

iCloseSocket = closesocket(TCPClientSocket);
if (iCloseSocket == SOCKET_ERROR)
{
cout<<"Closing Socket Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}


iWsaCleanup = WSACleanup();
if (iWsaCleanup == SOCKET_ERROR)
{
cout<<"CleanUp Fun Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
system("PAUSE");
return 0;
}


int64_t FileSize(const char* name)
{
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (!GetFileAttributesEx(name, GetFileExInfoStandard, &fad))
        return -1; // error condition, could call GetLastError to find out more
    LARGE_INTEGER size;
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;
    return size.QuadPart;
}


void ana_menu(){
    system("cls");
    puts("1) MUZIK GONDER");
    puts("2) MUZIK SIL");
    puts("3) MUZIK LISTESINI INDIR");
}

int DosyaGonder(const char* music_name,char* SenderBuffer,int SenderBufferSize,SOCKET TCPClientSocket){
    char *file_size = (char*)malloc(20);
    strcpy(SenderBuffer,"A");//add music
    strcat(SenderBuffer,music_name);//music name
    strcat(SenderBuffer,"|");
    int64_t dosya_boyutu = FileSize(music_name);
    if(dosya_boyutu == -1){return -1;}//İşlem başarısız böyle bir dosya yok!
    strcat(SenderBuffer,lltoa(dosya_boyutu,file_size,10));//size of music
    free(file_size);
    if (send(TCPClientSocket,SenderBuffer,SenderBufferSize,0) == SOCKET_ERROR)
    {return -2;}

    if (recv(TCPClientSocket,SenderBuffer,SenderBufferSize,0) == SOCKET_ERROR)
    {return -2;}
    if(strcmp(SenderBuffer,"OK") != 0){
        return -3;
    }
    FILE *original = fopen(music_name,"rb");
    if( !original){return -1;}

    char c;
    for(int i = 0;i<dosya_boyutu;i++){
        c = fgetc(original);
        send(TCPClientSocket,&c,1,0);
    }
    fclose(original);
    return dosya_boyutu;
}
