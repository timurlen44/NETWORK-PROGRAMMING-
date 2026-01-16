/*
 * TcpServer.h
 *
 *  Created on: Jul 4, 2025
 *      Author: ayoff
 */

#ifndef TCPSERVER_TCPSERVER_H_
#define TCPSERVER_TCPSERVER_H_

#include "stdint.h"

/**
 * @brief  Initializes the tcp server
 * @param  port_u16 states the server port
 * @param  void (*handleReceivedTcpServerPacketFuncPtr)(const uint8_t* const tcpServerDataBuffer, const uint16_t receivedDataLength_u16) is a function pointer that will be called every time when tcp server received whole packet from tcp client
 * @retval None
 */
bool initEthernetTcpServer(uint16_t port_u16, void (*handleReceivedTcpServerPacketFuncPtr)(const uint8_t* const tcpServerDataBuffer, const uint16_t receivedDataLength_u16));

#endif /* TCPSERVER_TCPSERVER_H_ */
