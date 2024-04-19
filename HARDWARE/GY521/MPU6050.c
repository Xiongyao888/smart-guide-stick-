#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"
#include "usart.h"


#define MPU6050_ADDRESS		0xD0  //д



void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();//������ʼ�ź�
	MyI2C_SendByte(MPU6050_ADDRESS);//����mpu6050�豸�ż���дλ��д��
	MyI2C_ReceiveAck();//�ȴ��ӻ���Ӧ
	MyI2C_SendByte(RegAddress);//����Ҫд�����ݵļĴ����ĵ�ַ
	MyI2C_ReceiveAck();//�ȴ��ӻ���Ӧ
	MyI2C_SendByte(Data);//��������
	MyI2C_ReceiveAck();//�ȴ��ӻ���Ӧ
	MyI2C_Stop();//����ֹͣ�ź�
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();//������ʼ�ź�
	MyI2C_SendByte(MPU6050_ADDRESS);//����mpu6050�豸�ż���дλ��д��
	MyI2C_ReceiveAck();//�ȴ��ӻ���Ӧ
	MyI2C_SendByte(RegAddress);//����Ҫд�����ݵļĴ����ĵ�ַ����ʱmpu6050�ڲ��Ĵ���ָ��ͻ�ָ�������ַ�������ٽ��з�����ʼ�źţ����·�
	                           //���ҷ���mpu6050�豸�ż���дλ������
	MyI2C_ReceiveAck();//�ȴ��ӻ���Ӧ
	MyI2C_Start();//������ʼ��Ϊ��������׼��
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);//����mpu6050�豸�ż���дλ������
	MyI2C_ReceiveAck();//�ȴ��ӻ���Ӧ
	Data = MyI2C_ReceiveByte();//��ȡ��λ����
	MyI2C_SendAck(1);//��ӻ����Ͳ����ٶ����ݵ���Ӧ
	MyI2C_Stop();//����ֹͣ�ź�
	
	return Data;
}

/*
**********************************************
**������  ��float Mpu6050AccelAngle(int8 dir)
**�������ܣ�������ٶȴ��������������ֵ
**            ��ΧΪ2gʱ�������ϵ��16384 LSB/g
**            �ǶȽ�Сʱ��x=sinx�õ��Ƕȣ����ȣ�, deg = rad*180/3.14
**            ��Ϊx>=sinx,�ʳ���1.2�ʵ��Ŵ�
**���ز��������������ֵ
**���������dir - ��Ҫ�����ķ���
**           ACCEL_XOUT - X����
**           ACCEL_YOUT - Y����
**           ACCEL_ZOUT - Z����
**********************************************
*/
float Mpu6050AccelAngle()
{
		uint8_t DataH, DataL;	
    float accel_agle; // ���������ֵ
    float result; // ����ֵ�������
		DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	  DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	  result = (DataH << 8) | DataL;
	  //printf("%f,",result);
    //result = (float)MPU6050_ReadReg(dir); // ������ǰ����ļ��ٶ�ֵ,ת��Ϊ������
    accel_agle = (result + 378)/16384; // ȥ�����ƫ��,����õ��Ƕȣ����ȣ�
    accel_agle = accel_agle*1.2*180/3.14;     //����ת��Ϊ��
    
    return accel_agle; // ���ز���ֵ
}

void MPU6050_Init(void)
{
	MyI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)//����Ǹ���MPU6050�ֲ���ж�ȡ�Ĵ���ֵ�Ĳ���
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}
