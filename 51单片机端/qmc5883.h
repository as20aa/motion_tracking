#ifndef __IIC_H_
#define __IIC_H_

//#include "lcd.h"
#include <reg52.h>
#include <intrins.h>
#define	Slave_Address   0x1a	 //定义器件在IIC总线中的从地址  read
 
 
sbit	SCL=P3^2;      //IIC时钟线
sbit	SDA=P3^3;      //IIC数据线

void Delay_5us();

void QMC5883_Start();
void QMC5883_Stop();
void QMC5883_SendACK(bit ack);
bit QMC5883_RecvACK();
void QMC5883_SendByte(int dat);
//u8 QMC5883_RecvByte();
//void QMC5883_RecvByte();
unsigned int QMC5883_Recvbyte();
void Single_Write_QMC5883(int REG_Address,int REG_data);
//u8 Single_Read_QMC5883(u8 REG_Address);
void Multiple_Read_QMC5883(void);
void Init_QMC5883();
#endif

















