/*
 * UdpClient.c
 *
 *  Created on: Jul 8, 2025
 *      Author: Emin
 */

#include "UdpClient.h"
#include "lwip.h"
#include "lwip/udp.h"

struct udp_pcb *g_ethernetUdpClientPcb = NULL;

bool initEthernetUdpClient(const uint8_t destinationIpAddr[4], const uint16_t destinationPort_u16)
{
	g_ethernetUdpClientPcb = udp_new ();
	if (g_ethernetUdpClientPcb == NULL)
	{
		return false;
	}

	bool status = setEthernetUdpClientDestinationIpAddrAndPort (destinationIpAddr, destinationPort_u16);
	return status;
}

bool sendEthernetUdpClientData(const uint8_t* const ethernetUdpClientBuffer, const uint16_t ethernetUdpClientBufferLength_u16)
{

	struct pbuf *txBuf = pbuf_alloc (PBUF_TRANSPORT, ethernetUdpClientBufferLength_u16, PBUF_RAM);
	if (txBuf != NULL)
	{
		pbuf_take (txBuf, ethernetUdpClientBuffer, ethernetUdpClientBufferLength_u16);
		udp_send (g_ethernetUdpClientPcb, txBuf);
		pbuf_free (txBuf);
		return true;
	}
	else
	{
		return false;
	}
}

bool setEthernetUdpClientDestinationIpAddrAndPort(const uint8_t destinationIpAddr[4], const uint16_t destinationPort_u16)
{
	ip_addr_t destIpAddr;
	IP_ADDR4( &destIpAddr, destinationIpAddr[0], destinationIpAddr[1], destinationIpAddr[2], destinationIpAddr[3]);
	err_t err = udp_connect (g_ethernetUdpClientPcb, &destIpAddr, destinationPort_u16);

	if (err == ERR_OK)
		return true;
	else
		return false;
}
