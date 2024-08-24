/*
  ***************************************************************************************************************
  ***************************************************************************************************************
  ***************************************************************************************************************

  File:		  udpServerRAW.c
  Author:     ControllersTech.com
  Updated:    Jul 23, 2021

  ***************************************************************************************************************
  Copyright (C) 2017 ControllersTech.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ***************************************************************************************************************
*/


#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "udpServerRAW.h"


extern uint8_t IP_ADDRESS[4];

uint16_t clientPort = 0;
uint8_t clientIPAddr[4] = {0};
uint32_t receivedPacketLength_u16 = 0;
uint8_t data[50];

void UDPReceiveCallBack(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);


/* IMPLEMENTATION FOR UDP Server :   source:https://www.geeksforgeeks.org/udp-server-client-implementation-c/

1. Create UDP socket.
2. Bind the socket to server address.
3. Wait until datagram packet arrives from client.
4. Process the datagram packet and send a reply to client.
5. Go back to Step 3.
*/

void udpServer_init(void)
{
	// UDP Control Block structure
   struct udp_pcb *upcb;
   err_t err;

   /* 1. Create a new UDP control block  */
   upcb = udp_new();

   /* 2. Bind the upcb to the local port */
   ip_addr_t myIPADDR;
   IP_ADDR4(&myIPADDR, IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]);

   err = udp_bind(upcb, &myIPADDR, 7);  // 7 is the server UDP port


   /* 3. Set a receive callback for the upcb */
   if(err == ERR_OK)
   {
	   udp_recv(upcb, UDPReceiveCallBack, NULL);
   }
   else
   {
	   udp_remove(upcb);
   }
}


bool initUDPServer(uint8_t *ipAddr, uint16_t port)
{

   bool status = false;
   struct udp_pcb *upcb;
   err_t err;

   upcb = udp_new();

   ip_addr_t IPADDR;
   IP_ADDR4(&IPADDR, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);

   err = udp_bind(upcb, &IPADDR, port);

   if(err == ERR_OK)
   {
	   udp_recv(upcb, UDPReceiveCallBack, NULL);
	   status = true;
   }
   else
   {
	   udp_remove(upcb);
	   status = false;
   }

   return status;
}

// udp_receive_callback will be called, when the client sends some data to the server
/* 4. Process the datagram packet and send a reply to client. */
void UDPReceiveCallBack(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	/* Get the IP of the Client */
	receivedPacketLength_u16 = p->tot_len;
	clientPort = port;
	memcpy(data,p->payload,receivedPacketLength_u16);

	/*
	uint8_t* dataP_c = (uint8_t*)p->payload;
	for(uint16_t i = 0; i < 50 ; i++)
	{
		data[i] = dataP_c[i];
	}
	*/

	clientIPAddr[0] = ip4_addr1(addr);
	clientIPAddr[1] = ip4_addr2(addr);
	clientIPAddr[2] = ip4_addr3(addr);
	clientIPAddr[3] = ip4_addr4(addr);

	udp_disconnect(upcb);
	pbuf_free(p);
}

