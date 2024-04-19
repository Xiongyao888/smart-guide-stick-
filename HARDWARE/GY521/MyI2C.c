#include "stm32f10x.h"                  // Device header
#include "delay.h"

//PB10��PB11
void MyI2C_W_SCL(uint8_t BitValue)//��������������дio�ڷ�װ��������ǿ�ɶ���
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

void MyI2C_Init(void)//���iic������gpio��ʼ����ע���ǿ�©���
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

void MyI2C_Start(void)//������ʼ�ź�
{
	MyI2C_W_SDA(1);//ȷ��SCL,SDA���ͷţ�Ȼ��������SDA��������SCL
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)//����ֹͣ�ź�
{
	MyI2C_W_SDA(0);//��ʱSCLһ��Ϊ�ͣ���������SDA,Ȼ�����ͷ�SCL,���ͷ�SDA
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)//ͨ��SDA����һ���ֽ�
{
	uint8_t i;//����˺���ʱSCLΪ�͵�ƽ����ʱ������SDA�������ݣ�Ȼ������SCL���ӻ��ͻ��ȡ���ݣ�ѭ������8λ
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)//ͨ��SDA��ȡһ���ֽڣ��ɴӻ�����
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);//�����ͷ�SDA���ôӻ�����SDA����Ȩ
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}//��SCLΪ�ߵ�ƽ�ڼ䣬�������Դ�SDA�ж�ȡ�ӻ����͵����ݣ�ѭ������8λ
		MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)//����������Ӧ�ź�
{
	MyI2C_W_SDA(AckBit);//����˺���ʱ��SCLΪ�͵�ƽ����ʱ��SDAд�����ݣ�Ȼ������SCL���ӻ��ͻ��ȡ����
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)//���մӻ���Ӧ
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);//�����ͷ�SDA���ôӻ�����SDA����Ȩ
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();//��SCLΪ�ߵ�ƽ�ڼ䣬�������Դ�SDA�ж�ȡ�ӻ����͵�����
	MyI2C_W_SCL(0);
	return AckBit;
}
