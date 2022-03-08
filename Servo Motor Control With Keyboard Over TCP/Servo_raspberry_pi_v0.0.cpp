#include <chrono>
#include <wiringPi.h>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080
#define ServoPin 18

volatile int servo1_aci = 150;
int main()
{
wiringPiSetupGpio();
pinMode (ServoPin, PWM_OUTPUT) ;
pwmSetMode (PWM_MODE_MS);
pwmSetRange (2000);//20ms
pwmSetClock (192);
pwmWrite(ServoPin,servo1_aci);
printf("Servo açısı = %d\n",servo1_aci);

int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    while(1){
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    puts("Socket created");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    puts("Socket Listen.");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    puts("Client connected.");
    while(1){
    read( new_socket , buffer, 1024);
    puts(buffer);
    if(buffer[0] == '1'){servo1_aci-=10;}
    if(servo1_aci < 40){servo1_aci = 40;}

    if(buffer[0] == '2'){servo1_aci+=10;}
    if(servo1_aci > 250){servo1_aci = 250;}

    pwmWrite(18,servo1_aci);
    printf("servo angle = %d\n",servo1_aci);

    memset(buffer,'\0',1024);
    }
    }
  return 0;
}
