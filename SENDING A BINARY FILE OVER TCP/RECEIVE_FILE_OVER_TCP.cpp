#include<winsock.h>
#include<iostream>
#include<string.h>
#define PORT 54000
#define MAX_SIZE 1024
const char* path ="C:\\Users\\USERNAME\\Documents\\";
char PathAndName[100];
char music_name[50];
char FileSizeArr[20];
char SenderBuffer[MAX_SIZE];
char RecvBuffer[MAX_SIZE];
int64_t dosya_boyutu = 0;
using namespace std;
void AddMusicCondition();
int64_t FileSize(const char* name);
int main()
{
 cout<<"\t\t------ TCP SERVER ---------"<<endl;
 cout<<endl;
 WSADATA Winsockdata;
 int iWsaStartup;
 int iWsaCleanup;
 SOCKET TCPServerSocket;
 int iCloseSocket;
 struct sockaddr_in  TCPServerAdd;
 struct sockaddr_in  TCPClientAdd;
 int iTCPClientAdd    = sizeof(TCPClientAdd);
 int iBind ;
 int iListen;
 SOCKET sAcceptSocket;
 int iSend;
 int iSenderBuffer = MAX_SIZE;
 int iRecv;
 int iRecvBuffer = MAX_SIZE;
iWsaStartup = WSAStartup(MAKEWORD(2,2),&Winsockdata);
if (iWsaStartup!= 0 )
{
cout<<"WSAStartUp Failed"<<endl;
cout<<"Error Code - "<<WSAGetLastError()<<endl;
}
cout<<"WSAStartUp Success"<<endl;


TCPServerAdd.sin_family = AF_INET;
TCPServerAdd.sin_addr.s_addr = htons(INADDR_ANY);
TCPServerAdd.sin_port = htons(PORT);

TCPServerSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if (TCPServerSocket == INVALID_SOCKET)
{
cout<<"TCP Server Socket Creation Failed"<<endl;
cout<<"Error Code - "<<WSAGetLastError()<<endl;
}
cout<<"TCP Server Socket Creation Success"<<endl;

 iBind = bind(
        TCPServerSocket,
        (SOCKADDR*)&TCPServerAdd,
        sizeof(TCPServerAdd));


if (iBind == SOCKET_ERROR)
{
cout<<"Binding Failed"<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"Binding Success"<<endl;
iListen = listen(TCPServerSocket,2);
if (iListen == SOCKET_ERROR)
{
cout<<"Listen Fun Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
sAcceptSocket = accept(TCPServerSocket,(SOCKADDR*)&TCPClientAdd,&iTCPClientAdd);
if (sAcceptSocket == INVALID_SOCKET)
{
cout<<"Accept Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"Connection Accepted"<<endl;
memset(RecvBuffer,'\0',1024);
iRecv = recv(sAcceptSocket,RecvBuffer,iRecvBuffer,0);
if (iRecv == SOCKET_ERROR)
{
cout<<"Receive Data Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"DATA RECEIVED -> "<<RecvBuffer<<endl;
if(RecvBuffer[0] == 'A'){
    AddMusicCondition();
    cout<<"MUSIC NAME = "<<music_name<<endl;
    cout<<"File Size = "<<dosya_boyutu<<endl;

    strcpy(SenderBuffer,"OK");
    iSend =send(sAcceptSocket,SenderBuffer,iSenderBuffer,0);
    if (iSend == SOCKET_ERROR)
    {
        cout<<"Sending Failed "<<endl;
        cout<<"Error No-> "<<WSAGetLastError()<<endl;
    }

    FILE *copy = fopen(PathAndName,"wb");
    if(!copy)
    {
        puts("File error!");
        exit(1);
    }

// STEP-7 Recv Data from Client
char c;
for(int64_t i = 0;i<dosya_boyutu;i++){
iRecv = recv(sAcceptSocket,&c,1,0);
fputc((unsigned char)c,copy);
}
fclose(copy);
    cout<<"Dosya eklendi"<<endl;
}
iCloseSocket = closesocket(TCPServerSocket);
if (iCloseSocket == SOCKET_ERROR)
{
cout<<"Closing Socket Failed "<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
puts("Closing Socket SUCCESS");

iWsaCleanup = WSACleanup();
if (iWsaCleanup == SOCKET_ERROR)
{
cout<<"CleanUp Fun Failed "<<WSAGetLastError()<<endl;
cout<<"Error No-> "<<WSAGetLastError()<<endl;
}
cout<<"CleanUp Fun Success"<<endl;

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
void AddMusicCondition(){
    memset(music_name,'\0',50);
    memset(FileSizeArr,'\0',20);
    char *c = strchr(RecvBuffer, '|');
    int index = (int)(c - RecvBuffer);
    strncpy(music_name,RecvBuffer+1,index-1);
    strcpy(FileSizeArr,RecvBuffer+index+1);
    cout<<"music_name == "<<music_name<<endl;
    cout<<"DonguSayisi == "<<FileSizeArr<<endl;
    dosya_boyutu = atoll(FileSizeArr);
    cout<<"Dosya boyutu == "<<dosya_boyutu<<endl;
    strcpy(PathAndName,path);
    strcat(PathAndName,music_name);
}
