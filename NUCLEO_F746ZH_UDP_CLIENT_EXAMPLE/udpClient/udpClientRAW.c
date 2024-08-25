#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#include "stdio.h"
#include "string.h"

#include "udpClientRAW.h"

struct udp_pcb *upcb;
char buffer[100];
int counter = 0;

void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

bool initUDPClient(uint8_t clientIPAddr[4],uint16_t clientPort, uint8_t serverIPAddr[4],uint16_t serverPort)
{
	bool status = true;
	err_t err;

	upcb = udp_new();

	ip_addr_t myIPaddr;
	IP_ADDR4(&myIPaddr, clientIPAddr[0], clientIPAddr[1], clientIPAddr[2], clientIPAddr[3]);
	err = udp_bind(upcb, &myIPaddr, clientPort);

	if(err != ERR_OK)
	{
		status = false;
	}

	ip_addr_t destIPaddr;
	IP_ADDR4(&destIPaddr, serverIPAddr[0], serverIPAddr[1], serverIPAddr[2], serverIPAddr[3]);
	err = udp_connect(upcb, &destIPaddr, serverPort);

	if(err != ERR_OK)
	{
		status = false;
	}

	return status;

}

void sendUDPMessage()
{
  struct pbuf *txBuf;
  char data[100];
  err_t err;
  bool status = true;

  counter++;
  int len = sprintf(data, "sending UDP client message %d\n", counter);

  txBuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

  if (txBuf != NULL)
  {
    pbuf_take(txBuf, data, len);
    udp_send(upcb, txBuf);
    pbuf_free(txBuf);
  }
}

