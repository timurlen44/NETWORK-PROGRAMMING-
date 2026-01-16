/*
 * UdpClient.h
 *
 *  Created on: Jul 8, 2025
 *      Author: Emin
 */

#ifndef UDPCLIENT_UDPCLIENT_H_
#define UDPCLIENT_UDPCLIENT_H_

#include "stdint.h"
#include <stdbool.h>
/**
 * @brief initializes Udp Client
 * @param destinationIpAddr ip address for sending data
 * @param destinationPort_u16 port for sending data
 */
bool initEthernetUdpClient(const uint8_t destinationIpAddr[4], const uint16_t destinationPort_u16);
/**
 * @brief Sets destination udp server's ip address and port
 * @param destinationIpAddr ip address for sending data
 * @param destinationPort_u16 port for sending data
 */
bool setEthernetUdpClientDestinationIpAddrAndPort(const uint8_t destinationIpAddr[4], const uint16_t destinationPort_u16);

/**
 * @brief sends given data to udp server
 * @param ethernetUdpClientBuffer buffer in which the data to be sent is kept
 * @param ethernetUdpClientBufferLength_u16 the size of the data to be sent
 */
bool sendEthernetUdpClientData(const uint8_t* const ethernetUdpClientBuffer, const uint16_t ethernetUdpClientBufferLength_u16);

#endif /* UDPCLIENT_UDPCLIENT_H_ */
