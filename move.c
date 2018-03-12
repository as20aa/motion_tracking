#include "move.h"
#include "i2c.h"
#include "uart.h"

void move(unsigned char number,int position)
{
	SeriPushSend(0x55);
	SeriPushSend(0x55);
	SeriPushSend(0x08);
	SeriPushSend(0x03);
	SeriPushSend(0x01);
	SeriPushSend(0xE8);
	SeriPushSend(0x03);
	SeriPushSend(number);
	SeriPushSend(position&0xff);//µÍ8Î»ÌáÇ°
	SeriPushSend(position>>8);
}