/*
 * UdpServer.c
 *
 *  Created on: Jul 8, 2025
 *      Author: Emin
 */

#include "UdpServer.h"
#include "lwip.h"
#include "lwip/udp.h"

static void (*handleReceivedUdpServerPacket)(const uint8_t* const udpServerDataBuffer, const uint16_t receivedDataLength_u16) = NULL; //!< Function Pointer that is called whenever udp server receives whole udp client's packet
struct udp_pcb *g_ethernetUdpServerPcb = NULL;

static void ethernetUdpServerReceiveCallBack(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, u16_t port);

bool initEthernetUdpServer(const uint16_t port_u16, void (*handleReceivedUdpServerPacketFuncPtr)(const uint8_t* const udpServerDataBuffer, const uint16_t receivedDataLength_u16))
{
	if (handleReceivedUdpServerPacketFuncPtr == NULL)
	{
		return false;
	}
	handleReceivedUdpServerPacket = handleReceivedUdpServerPacketFuncPtr;

	g_ethernetUdpServerPcb = udp_new ();
	if (g_ethernetUdpServerPcb == NULL)
	{
		return false;
	}

	bool status = setEthernetUdpServerPort (port_u16);
	if (status)
	{
		udp_recv (g_ethernetUdpServerPcb, ethernetUdpServerReceiveCallBack, NULL);
		return true;
	}
	else
	{
		udp_remove (g_ethernetUdpServerPcb);
		return false;
	}

}
bool setEthernetUdpServerPort(const uint16_t port_u16)
{
	err_t err = udp_bind (g_ethernetUdpServerPcb, IP4_ADDR_ANY, port_u16);

	if (err == ERR_OK)
		return true;
	else
		return false;
}

static void ethernetUdpServerReceiveCallBack(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, u16_t port)
{
	handleReceivedUdpServerPacket (p->payload, p->tot_len);
	udp_disconnect (upcb);
	pbuf_free (p);
}
