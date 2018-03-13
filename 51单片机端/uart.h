#ifndef __UART_H__
#define __UART_H__
#include "reg52.h"
void init_uart(void);
void SeriPushSend(unsigned char send_data);
#endif