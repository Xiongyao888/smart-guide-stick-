#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"
#include "usart.h"


#define MPU6050_ADDRESS		0xD0  //写



void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();//发送起始信号
	MyI2C_SendByte(MPU6050_ADDRESS);//发送mpu6050设备号及读写位（写）
	MyI2C_ReceiveAck();//等待从机响应
	MyI2C_SendByte(RegAddress);//发送要写入数据的寄存器的地址
	MyI2C_ReceiveAck();//等待从机响应
	MyI2C_SendByte(Data);//发送数据
	MyI2C_ReceiveAck();//等待从机响应
	MyI2C_Stop();//发送停止信号
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();//发送起始信号
	MyI2C_SendByte(MPU6050_ADDRESS);//发送mpu6050设备号及读写位（写）
	MyI2C_ReceiveAck();//等待从机响应
	MyI2C_SendByte(RegAddress);//发送要写入数据的寄存器的地址，此时mpu6050内部寄存器指针就会指向这个地址，下面再进行发送起始信号，见下方
	                           //并且发送mpu6050设备号及读写位（读）
	MyI2C_ReceiveAck();//等待从机响应
	MyI2C_Start();//二次起始，为读数据做准备
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);//发送mpu6050设备号及读写位（读）
	MyI2C_ReceiveAck();//等待从机响应
	Data = MyI2C_ReceiveByte();//读取八位数据
	MyI2C_SendAck(1);//向从机发送不想再读数据的响应
	MyI2C_Stop();//发送停止信号
	
	return Data;
}

/*
**********************************************
**函数名  ：float Mpu6050AccelAngle(int8 dir)
**函数功能：输出加速度传感器测量的倾角值
**            范围为2g时，换算关系：16384 LSB/g
**            角度较小时，x=sinx得到角度（弧度）, deg = rad*180/3.14
**            因为x>=sinx,故乘以1.2适当放大
**返回参数：测量的倾角值
**传入参数：dir - 需要测量的方向
**           ACCEL_XOUT - X方向
**           ACCEL_YOUT - Y方向
**           ACCEL_ZOUT - Z方向
**********************************************
*/
float Mpu6050AccelAngle()
{
		uint8_t DataH, DataL;	
    float accel_agle; // 测量的倾角值
    float result; // 测量值缓存变量
		DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	  DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	  result = (DataH << 8) | DataL;
	  //printf("%f,",result);
    //result = (float)MPU6050_ReadReg(dir); // 测量当前方向的加速度值,转换为浮点数
    accel_agle = (result + 378)/16384; // 去除零点偏移,计算得到角度（弧度）
    accel_agle = accel_agle*1.2*180/3.14;     //弧度转换为度
    
    return accel_agle; // 返回测量值
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
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)//这就是根据MPU6050手册进行读取寄存器值的操作
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
