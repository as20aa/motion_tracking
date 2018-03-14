#include <REG52.H>	
#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library	
#include "i2c.h"//һ��Ҫ����Ŀ�ļ��а�������Ȼ�����һ��unsolved external defination��
#include "uart.h"
#include "move.h"
#include "robot_driver.h"

// ����MPU6050�ڲ���ַ
//****************************************
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
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
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I			0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define	SlaveAddress	0xD0	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ

//****************************************
//�������ͼ�����
//****************************************
unsigned char ge,shi,bai,qian,wan;
//****************************************
//��������
//****************************************
void  delay(unsigned int k);										//��ʱ						
//****************************************
//��ʱ
//****************************************
void delay(unsigned int k)	
{					
//��ʱ����	
	unsigned int i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<121;j++);
	}						
}

//**************************************
//�ϳ�����
//**************************************
void GetData(uchar REG_Address,unsigned char temp[16])
{
	//�Ӵ������ж�ȡ��Ϣ
	//��Ҫ���޸�temp�е�16��Ԫ�ص���Ӧ������
	Single_ReadI2C(REG_Address,temp);
	Single_ReadI2C(REG_Address+1,temp);
	
}
//ת������
void converse(int num)
{
	//ת������
	//���ԱȽϴ�����ݽ��л���
	//ÿһλ�����յ����ֽڴ洢
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
	//���ô��ڷ�������
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
	//��temp�н��յ���16������ת�����ܱ���֫����������İ˸�����������num��
	unsigned char i;
	for(i=0;i<8;i++)//���ݴ������������������ϵĴ�����˳��洢���������õ�������
		{
			num[i]=temp[7-i]<<8 | temp[15-i];
			num[i]=num[i]/17;//��Сnum�仯��Χ��ʹ����0~2000֮��䶯
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
//������
//*********************************************************
void main()
{ 
	int num[8];//���ڴ洢�ĸ����������ͻ���������
	unsigned char temp[16];
	init_uart();
	InitMPU6050();	
	
	while(1)
	{
		GetData(ACCEL_XOUT_H,temp);//��ȡ���ݴ洢��temp��
		transform(num,temp);//�����ݴ�temp��ת����num��
		
		//���������Ѿ���������֫�Ķ���������
		//��Ҫ�������ƫ�������Է�ֹ��ȡ�����������嶯��������
		//���������Լ�ƫ�����������������ļ����ڡ�����.txt��
		//num[2]=num[2]-num[0]+1500;
		//����ת������
		//���ռ��������ݴ��͸���λ��
		send_data(num);
		//right_leg(num);//���ȵ�����
		
		//left_leg(num);//���ȵ�����
		
		//left_hand(num);//���ֵ�����
		
		//right_hand(num);//���ֵ�����
		delay(100);
	}
}
