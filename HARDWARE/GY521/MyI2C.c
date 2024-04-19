#include "stm32f10x.h"                  // Device header
#include "delay.h"

//PB10、PB11
void MyI2C_W_SCL(uint8_t BitValue)//这三个函数将读写io口封装起来，增强可读性
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)BitValue);
	delay_us(10);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)BitValue);
	delay_us(10);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
	delay_us(10);
	return BitValue;
}

void MyI2C_Init(void)//软件iic的两个gpio初始化，注意是开漏输出
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

void MyI2C_Start(void)//发送起始信号
{
	MyI2C_W_SDA(1);//确保SCL,SDA都释放，然后先拉低SDA，再拉低SCL
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)//发送停止信号
{
	MyI2C_W_SDA(0);//此时SCL一定为低，所以拉低SDA,然后先释放SCL,再释放SDA
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)//通过SDA发送一个字节
{
	uint8_t i;//进入此函数时SCL为低电平，此时主机向SDA发送数据，然后拉高SCL，从机就会读取数据，循环发送8位
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)//通过SDA读取一个字节，由从机发送
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);//主机释放SDA，让从机掌握SDA控制权
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}//在SCL为高电平期间，主机可以从SDA中读取从机发送的数据，循环接收8位
		MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)//发送主机响应信号
{
	MyI2C_W_SDA(AckBit);//进入此函数时，SCL为低电平，此时向SDA写入数据，然后拉高SCL，从机就会读取数据
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)//接收从机响应
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);//主机释放SDA，让从机掌握SDA控制权
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();//在SCL为高电平期间，主机可以从SDA中读取从机发送的数据
	MyI2C_W_SCL(0);
	return AckBit;
}
