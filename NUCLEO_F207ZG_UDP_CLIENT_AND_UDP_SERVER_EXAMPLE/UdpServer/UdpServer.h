/*
 * UdpServer.h
 *
 *  Created on: Jul 8, 2025
 *      Author: Emin
 */

#ifndef UDPSERVER_UDPSERVER_H_
#define UDPSERVER_UDPSERVER_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief  Initializes the udp server
 * @param  port_u16 states the server port
 * @param  void (*handleReceivedUdpServerPacketFuncPtr)(const uint8_t* const udpServerDataBuffer, const uint16_t receivedDataLength_u16) is a function pointer that will be called every time when udp server received whole packet from udp client
 * @retval bool
 */
bool initEthernetUdpServer(const uint16_t port_u16, void (*handleReceivedUdpServerPacketFuncPtr)(const uint8_t* const udpServerDataBuffer, const uint16_t receivedDataLength_u16));

/**
 * brief sets the udp server's port
 * @param port_u16 states the server port
 */
bool setEthernetUdpServerPort(const uint16_t port_u16);

#endif /* UDPSERVER_UDPSERVER_H_ */
