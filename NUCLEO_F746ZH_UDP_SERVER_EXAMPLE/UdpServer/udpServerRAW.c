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

bool initUDPServer(uint16_t port)
{

   bool status = false;
   struct udp_pcb *upcb;
   err_t err;

   upcb = udp_new();
   err = udp_bind(upcb, IP4_ADDR_ANY, port);

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

void UDPReceiveCallBack(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	receivedPacketLength_u16 = p->tot_len;
	clientPort = port;
	memcpy(data,p->payload,receivedPacketLength_u16);
	
	clientIPAddr[0] = ip4_addr1(addr);
	clientIPAddr[1] = ip4_addr2(addr);
	clientIPAddr[2] = ip4_addr3(addr);
	clientIPAddr[3] = ip4_addr4(addr);

	udp_disconnect(upcb);
	pbuf_free(p);
}

