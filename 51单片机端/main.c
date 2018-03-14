#include <REG52.H>	
#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library	
#include "i2c.h"//一定要在项目文件中包含，不然会出现一个unsolved external defination！
#include "uart.h"
#include "move.h"
#include "robot_driver.h"

// 定义MPU6050内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I			0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取

//****************************************
//定义类型及变量
//****************************************
unsigned char ge,shi,bai,qian,wan;
//****************************************
//函数声明
//****************************************
void  delay(unsigned int k);										//延时						
//****************************************
//延时
//****************************************
void delay(unsigned int k)	
{					
//延时函数	
	unsigned int i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<121;j++);
	}						
}

//**************************************
//合成数据
//**************************************
void GetData(uchar REG_Address,unsigned char temp[16])
{
	//从传感器中读取信息
	//主要是修改temp中的16个元素的相应的数据
	Single_ReadI2C(REG_Address,temp);
	Single_ReadI2C(REG_Address+1,temp);
	
}
//转换数据
void converse(int num)
{
	//转换函数
	//即对比较大的数据进行换算
	//每一位均按照单个字节存储
	wan=num/10000;
	num=num%10000;
	qian=num/1000;
	num=num%1000;
	bai=num/100;
	num=num%100;
	shi=num/10;
	ge=num%10;
}

void send(int num[8])
{
	//利用串口发送数据
	int i;
	for(i=0;i<8;i++)
		{
			SBUF=i+0x30;
			while(!TI);TI=0;
			SBUF=':';
			while(!TI);TI=0;
			if(num[i]<0)
			{
				num[i]=-num[i];
				SBUF='-';
				while(!TI);TI=0;
			}
			else
			{
				SBUF='+';
				while(!TI);TI=0;
			}
			converse(num[i]);
			SBUF=wan+0x30;
			while(!TI);TI=0;
			SBUF=qian+0x30;
			while(!TI);TI=0;
			SBUF=bai+0x30;
			while(!TI);TI=0;
			SBUF=shi+0x30;
			while(!TI);TI=0;
			SBUF=ge+0x30;
			while(!TI);TI=0;
			SBUF=0x0d;
			while(!TI);TI=0;
			SBUF=0x0a;
			while(!TI);TI=0;
		}
}

void transform(int num[8],unsigned char temp[16])
{
	//将temp中接收到的16个数据转换成能被四肢动作所处理的八个传感器数据num中
	unsigned char i;
	for(i=0;i<8;i++)//数据处理，按照连接在引脚上的传感器顺序存储传感器所得到的数据
		{
			num[i]=temp[7-i]<<8 | temp[15-i];
			num[i]=num[i]/17;//缩小num变化范围，使其在0~2000之间变动
		}
}
void send_data(unsigned char num[8])
{
	int i;
	for(i=0;i<8;i++)
	{
		SBUF=num[i];
		while(!TI)TI=0;
	}
	SBUF=0X0D;
	while(!TI)TI=0;
	SBUF=0X0A;
	while(!TI)TI=0;
}

//*********************************************************
//主程序
//*********************************************************
void main()
{ 
	int num[8];//用于存储四个传感器传送回来的数据
	unsigned char temp[16];
	init_uart();
	InitMPU6050();	
	
	while(1)
	{
		GetData(ACCEL_XOUT_H,temp);//获取数据存储于temp中
		transform(num,temp);//将数据从temp中转换到num中
		
		//修正程序已经包含在四肢的动作函数中
		//需要另外加上偏移量，以防止读取的数据与人体动作不符合
		//修正程序以及偏移量见本程序所在文件夹内“修正.txt”
		//num[2]=num[2]-num[0]+1500;
		//数据转换部分
		//将收集到的数据传送给上位机
		send_data(num);
		//right_leg(num);//右腿的驱动
		
		//left_leg(num);//左腿的驱动
		
		//left_hand(num);//左手的驱动
		
		//right_hand(num);//右手的驱动
		delay(100);
	}
}
