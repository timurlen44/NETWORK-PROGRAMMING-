/*
  ***************************************************************************************************************
  ***************************************************************************************************************
  ***************************************************************************************************************

  File:		  udpClientRAW.h
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


#ifndef INC_UDPCLIENTRAW_H_
#define INC_UDPCLIENTRAW_H_
#include "stdbool.h"

void sendUDPMessage(void);
bool initUDPClient(uint8_t clientIPAddr[4],uint16_t clientPort, uint8_t serverIPAddr[4],uint16_t serverPort);


#endif /* INC_UDPCLIENTRAW_H_ */
