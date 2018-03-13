#include "i2c.h"
#include "reg52.h"



//**************************************
//延时5微秒(STC90C52RC@12M)
//不同的工作环境,需要调整此函数
//当改用1T的MCU时,请调整此延时函数
//**************************************
void Delay5us(void)
{
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}
//**************************************
//I2C起始信号
//**************************************
void I2C_Start(void)
{
    P1 = 0XFF;                    //拉高数据线
    P0 = 0XFF;                    //拉高时钟线
    Delay5us();                 //延时
    P1 = 0;                    //产生下降沿
    Delay5us();                 //延时
    P0 = 0;                    //拉低时钟线
}
//**************************************
//I2C停止信号
//**************************************
void I2C_Stop()
{
    P1 = 0;                    //拉低数据线
    P0 = 0XFF;                    //拉高时钟线
    Delay5us();                 //延时
    P1 = 0XFF;                    //产生上升沿
    Delay5us();                 //延时
}
//**************************************
//I2C发送应答信号
//入口参数:ack (0:ACK 1:NAK)
//**************************************
void I2C_SendACK(bit ack)
{
	if(ack==1)
    P1 = 0XFF;                  //写应答信号
	else
		P1 = 0;
    P0 = 0XFF;                    //拉高时钟线
    Delay5us();                 //延时
    P0 = 0;                    //拉低时钟线
    Delay5us();                 //延时
}
//**************************************
//I2C接收应答信号
//**************************************
unsigned char I2C_RecvACK()
{
	unsigned char buf;
    P0 = 0XFF;                    //拉高时钟线
    Delay5us();                 //延时
    buf = P1;                   //读应答信号
    P0 = 0;                    //拉低时钟线
    Delay5us();                 //延时
    return buf;
}
//**************************************
//向I2C总线发送一个字节数据
//**************************************
void I2C_SendByte(uchar dat)
{
    uchar i;
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        if(CY==1)               //送数据口
					P1 = 0XFF;
				else
					P1 = 0;
        P0 = 0XFF;                //拉高时钟线
        Delay5us();             //延时
        P0 = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    I2C_RecvACK();
}
//**************************************
//从I2C总线接收一个字节数据
//**************************************
void I2C_RecvByte(unsigned char* buf)//buf由上一级函数提供
{
    uchar i;
    P1 = 0XFF;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        P0 = 0XFF;                //拉高时钟线
        Delay5us();             //延时
        *(buf+i) = P1;             //读数据               
        P0 = 0;                //拉低时钟线
        Delay5us();             //延时
    }
}
//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteI2C(uchar REG_Address,uchar REG_data)
{
    I2C_Start();                  //起始信号
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
    I2C_SendByte(REG_Address);    //内部寄存器地址，
    I2C_SendByte(REG_data);       //内部寄存器数据，
    I2C_Stop();                   //发送停止信号
}
//**************************************
//从I2C设备读八个字节数据
//**************************************
void Single_ReadI2C(uchar REG_Address,uchar temp[16])
{
	//从传感器读取一次数据
	//读取的数据根据传入的REG_Address决定
	//返回的是一个大小为16的uchar数组
	unsigned char i,j,k;
	uchar buf[8];
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress);    //发送设备地址+写信号
	I2C_SendByte(REG_Address);     //发送存储单元地址，从0开始	
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	I2C_RecvByte(buf);       //读出寄存器数据
	I2C_SendACK(1);                //接收应答信号
	I2C_Stop();                    //停止信号
	//将buf转化，将buf全部转化为temp
	////////////////////////
	//处理得到的八个字节的数据
	//问题就在将buf转换成temp上！
	////////////////////////
	if(REG_Address&0x01)//当为奇数时即为高八位的数据
	{
		for(j=0;j<8;j++)
		{
			for(i=0;i<8;i++)
			{
				temp[j]<<=1;
				buf[i]<<=1;//注意使用的顺序！如果是使用在TEMP做位移前则CY表示的时TEMP位移的结果！
				if(CY==1)
				temp[j]=temp[j]+1;
			}
		}//temp[j]代表的是第j个传感器的数据
	}
	else
	{
		for(j=8;j<16;j++)
		{
			for(i=0;i<8;i++)
			{
				temp[j]<<=1;
				buf[i]<<=1;//注意使用的顺序！如果是使用在TEMP做位移前则CY表示的时TEMP位移的结果！
				if(CY==1)
				temp[j]=temp[j]+1;
			}
		}//temp[j]代表的是第j个传感器的数据
	}
	
}
//MPU6050操作函数
//**************************************
//初始化MPU6050
//**************************************
void InitMPU6050()
{
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07);
	Single_WriteI2C(CONFIG, 0x06);
	Single_WriteI2C(GYRO_CONFIG, 0x18);
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}