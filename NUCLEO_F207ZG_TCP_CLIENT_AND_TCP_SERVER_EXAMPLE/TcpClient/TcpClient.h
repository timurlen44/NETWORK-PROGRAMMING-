/*
 * TcpClient.h
 *
 *  Created on: Jul 4, 2025
 *      Author: ayoff
 */

#ifndef TCPCLIENT_TCPCLIENT_H_
#define TCPCLIENT_TCPCLIENT_H_

#include "main.h"
#include <stdbool.h>
#include <stdint.h>
#include "lwip.h"
#include "lwip/tcp.h"

#define DEST_IP_ADDR0   (uint8_t) 192
#define DEST_IP_ADDR1   (uint8_t) 168
#define DEST_IP_ADDR2   (uint8_t) 2
#define DEST_IP_ADDR3   (uint8_t) 110

#define DEST_PORT       (uint16_t) 23

#define IP_ADDR0   (uint8_t) 192
#define IP_ADDR1   (uint8_t) 168
#define IP_ADDR2   (uint8_t) 2
#define IP_ADDR3   (uint8_t) 55

#define NETMASK_ADDR0   (uint8_t) 255
#define NETMASK_ADDR1   (uint8_t) 255
#define NETMASK_ADDR2   (uint8_t) 255
#define NETMASK_ADDR3   (uint8_t) 0

#define GW_ADDR0   (uint8_t) 192
#define GW_ADDR1   (uint8_t) 168
#define GW_ADDR2   (uint8_t) 2
#define GW_ADDR3   (uint8_t) 1

typedef enum TCP_CLIENT_CURRENT_STATUS_ENUM
{
	TCP_CLIENT_CURRENT_STATUS_THERE_IS_NO_AVAILABLE_TCP_PCB = 0,
	TCP_CLIENT_CURRENT_STATUS_CLIENT_HAS_NOT_CONNECTED_YET,
	TCP_CLIENT_CURRENT_STATUS_READY_TO_SEND_NEW_DATA,
} TcpClientCurrentStatusEnum_e;

bool initEthernetTcpClient(const uint8_t destinationIpAddr[4], const uint16_t destinationPort);
bool isEthernetTcpClientAvailableForSending(void);
bool sendEthernetTcpClientData(const uint8_t* const data, const uint16_t size);
bool setEthernetTcpClientDestinationIpAddrAndPort(const uint8_t destinationIpAddr[4], const uint16_t destinationPort);

#endif /* TCPCLIENT_TCPCLIENT_H_ */
