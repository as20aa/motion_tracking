#include "uart.h"

void init_uart(void)
{
	TMOD=0x21;				
	TH1=0xfd;				
	TL1=0xfd;		
		
	SCON=0x50;
	PS=1;      //�����ж���Ϊ�����ȼ���
	TR0=1;	   //������ʱ��			
	TR1=1;
	ET0=1;     //�򿪶�ʱ��0�ж�			
	ES=1;	
	EA=1;
}
void  SeriPushSend(unsigned char send_data)
{
    SBUF=send_data;  
	while(!TI);TI=0;	  
}

