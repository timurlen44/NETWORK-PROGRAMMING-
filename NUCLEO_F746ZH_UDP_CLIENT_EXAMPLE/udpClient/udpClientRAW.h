#ifndef INC_UDPCLIENTRAW_H_
#define INC_UDPCLIENTRAW_H_
#include "stdbool.h"

void sendUDPMessage(void);
bool initUDPClient(uint8_t clientIPAddr[4],uint16_t clientPort, uint8_t serverIPAddr[4],uint16_t serverPort);


#endif /* INC_UDPCLIENTRAW_H_ */
