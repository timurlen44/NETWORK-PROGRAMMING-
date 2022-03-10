#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
using namespace std;
void find_sum(string& sum,string& website_HTML);



int main( void ){

WSADATA wsaData;
SOCKET Socket;
SOCKADDR_IN SockAddr;
int lineCount=0;
int rowCount=0;
struct hostent *host;
locale local;
char buffer[10000];
int i = 0 ;
int nDataLength;
string website_HTML;

// website url
string url = "127.0.0.1";
#define PORT 80
//HTTP GET
string num1;
string num2;
string sum;

cout<<"Enter num1:"<<endl;
cin>>num1;

cout<<"Enter num2:"<<endl;
cin>>num2;

cout<<"Please wait !!!"<<endl;

string get_http = "GET /sum/?num1="+num1+"&num2="+num2+" HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        cout << "WSAStartup failed.\n";
        system("pause");
        //return 1;
    }

    Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    //host = gethostbyname("169.254.255.101");
    host = gethostbyname(url.c_str());

    SockAddr.sin_port=htons(PORT);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        cout << "Could not connect";
        system("pause");
        //return 1;
    }

    // send GET / HTTP
    send(Socket,get_http.c_str(), strlen(get_http.c_str()),0 );

    // recieve html
    while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r'){

            website_HTML+=buffer[i];
            i += 1;
        }
    }

    closesocket(Socket);
    WSACleanup();


    find_sum(sum,website_HTML);
    // pause
    cout<<"\n\nPress ANY key to close.\n\n";
    cin.ignore(); cin.get();
 return 0;
}

void find_sum(string& sum,string& website_HTML){
    cout <<"Received data: "<<endl<<website_HTML<<endl<<"----------------------------------------------"<<endl;
    size_t first = website_HTML.find("#");
    size_t last = website_HTML.find_last_of("#");
    for(int i = first+1; i<last;i++){
        sum += website_HTML[i];
    }
    cout<<"sum = "<<sum<<endl;
    return;
}
