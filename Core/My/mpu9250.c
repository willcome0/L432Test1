#include "mpu9250.h"
#include "i2c.h"


unsigned char TX_DATA[4];
unsigned char BUF[10];
char  test=0;
short T_X,T_Y,T_Z,T_T;
void Single_Write(uint8_t slave_add, uint8_t reg_add, uint8_t data)
{
    uint8_t write_data[2] = {reg_add, data};
    HAL_I2C_Master_Transmit(I2C_Device, slave_add, write_data, 2, 0xFFFF);
}

uint8_t Single_Read(uint8_t slave_add, uint8_t reg_add)
{
    uint8_t data = 0;
    HAL_I2C_Mem_Read(I2C_Device, slave_add, reg_add, 1, &data, 1, 0xFFFF);
    return data;
}

void Init_MPU9250(void)
{
    Single_Write(GYRO_ADDRESS,PWR_MGMT_1, 0x00);//唤醒MPU6050 	//解除休眠状态
    Single_Write(GYRO_ADDRESS,SMPLRT_DIV, 0x07);//设置采样率50Hz

    Single_Write(GYRO_ADDRESS,GYRO_CONFIG, 0x18);//陀螺仪传感器量程
    Single_Write(GYRO_ADDRESS,ACCEL_CONFIG, 0x01);//加速度计量程
    Single_Write(GYRO_ADDRESS,CONFIG, 0x06);//设置采样率
}

void READ_MPU9250_ACCEL(void)
{
    BUF[0]=Single_Read(ACCEL_ADDRESS,ACCEL_XOUT_L);
    BUF[1]=Single_Read(ACCEL_ADDRESS,ACCEL_XOUT_H);
    T_X = (BUF[1]<<8)|BUF[0];
    T_X/=164; 						   //读取计算X轴数据

    BUF[2]=Single_Read(ACCEL_ADDRESS,ACCEL_YOUT_L);
    BUF[3]=Single_Read(ACCEL_ADDRESS,ACCEL_YOUT_H);
    T_Y = (BUF[3]<<8)|BUF[2];
    T_Y/=164; 						   //读取计算Y轴数据
    BUF[4]=Single_Read(ACCEL_ADDRESS,ACCEL_ZOUT_L);
    BUF[5]=Single_Read(ACCEL_ADDRESS,ACCEL_ZOUT_H);
    T_Z = (BUF[5]<<8)|BUF[4];
    T_Z/=164; 					       //读取计算Z轴数据
}

void READ_MPU9250_GYRO(void)
{

    BUF[0]=Single_Read(GYRO_ADDRESS,GYRO_XOUT_L);
    BUF[1]=Single_Read(GYRO_ADDRESS,GYRO_XOUT_H);
    T_X=	(BUF[1]<<8)|BUF[0];
    T_X/=16.4; 						   //读取计算X轴数据

    BUF[2]=Single_Read(GYRO_ADDRESS,GYRO_YOUT_L);
    BUF[3]=Single_Read(GYRO_ADDRESS,GYRO_YOUT_H);
    T_Y=	(BUF[3]<<8)|BUF[2];
    T_Y/=16.4; 						   //读取计算Y轴数据
    BUF[4]=Single_Read(GYRO_ADDRESS,GYRO_ZOUT_L);
    BUF[5]=Single_Read(GYRO_ADDRESS,GYRO_ZOUT_H);
    T_Z=	(BUF[5]<<8)|BUF[4];
    T_Z/=16.4; 					       //读取计算Z轴数据
}

void READ_MPU9250_MAG(void)
{
    Single_Write(GYRO_ADDRESS,0x37,0x02);//turn on Bypass Mode
    HAL_Delay(10);
    Single_Write(MAG_ADDRESS,0x0A,0x01);
    HAL_Delay(10);
    BUF[0]=Single_Read (MAG_ADDRESS,MAG_XOUT_L);
    BUF[1]=Single_Read (MAG_ADDRESS,MAG_XOUT_H);
    T_X=(BUF[1]<<8)|BUF[0];

    BUF[2]=Single_Read(MAG_ADDRESS,MAG_YOUT_L);
    BUF[3]=Single_Read(MAG_ADDRESS,MAG_YOUT_H);
    T_Y=	(BUF[3]<<8)|BUF[2];
    //读取计算Y轴数据

    BUF[4]=Single_Read(MAG_ADDRESS,MAG_ZOUT_L);
    BUF[5]=Single_Read(MAG_ADDRESS,MAG_ZOUT_H);
    T_Z=	(BUF[5]<<8)|BUF[4];
    //读取计算Z轴数据
}

void READ_MPU9250_TEMP(void)
{
    BUF[0]=Single_Read(ACCEL_ADDRESS, TEMP_OUT_L);
    BUF[1]=Single_Read(ACCEL_ADDRESS, TEMP_OUT_L);
    T_T=	(BUF[1]<<8)|BUF[0];
    T_T = 35+ ((double) (T_T + 13200)) / 280;
}