/**			                                                    
		   ____                    _____ _______ _____       @塔克创新
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   机器人控制器
									 
  ****************************************************************************** 
  *           
  * 版权所有： @塔克创新  版权所有，盗版必究
  * 公司网站： www.xtark.cn   www.tarkbot.com
  * 淘宝店铺： https://xtark.taobao.com  
  * 塔克微信： 塔克创新（关注公众号，获取最新更新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  MPU6050 操作
  *
  ******************************************************************************
  */
  
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_MPU6050_H
#define __AX_MPU6050_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define	MPU6050_ADDR    0x68    //MPU6050地址

//MPU6050寄存器地址
#define MPU6050_SELF_TESTX_REG		0X0D	//自检寄存器X
#define MPU6050_SELF_TESTY_REG		0X0E	//自检寄存器Y
#define MPU6050_SELF_TESTZ_REG		0X0F	//自检寄存器Z
#define MPU6050_SELF_TESTA_REG		0X10	//自检寄存器A
#define MPU6050_SAMPLE_RATE_REG		0X19	//采样频率分频器
#define MPU6050_CFG_REG				0X1A	//配置寄存器
#define MPU6050_GYRO_CFG_REG		0X1B	//陀螺仪配置寄存器
#define MPU6050_ACCEL_CFG_REG		0X1C	//加速度计配置寄存器
#define MPU6050_MOTION_DET_REG		0X1F	//运动检测阀值设置寄存器
#define MPU6050_FIFO_EN_REG			0X23	//FIFO使能寄存器
#define MPU6050_I2CMST_CTRL_REG		0X24	//IIC主机控制寄存器
#define MPU6050_I2CSLV0_ADDR_REG	0X25	//IIC从机0器件地址寄存器
#define MPU6050_I2CSLV0_REG			0X26	//IIC从机0数据地址寄存器
#define MPU6050_I2CSLV0_CTRL_REG	0X27	//IIC从机0控制寄存器
#define MPU6050_I2CSLV1_ADDR_REG	0X28	//IIC从机1器件地址寄存器
#define MPU6050_I2CSLV1_REG			0X29	//IIC从机1数据地址寄存器
#define MPU6050_I2CSLV1_CTRL_REG	0X2A	//IIC从机1控制寄存器
#define MPU6050_I2CSLV2_ADDR_REG	0X2B	//IIC从机2器件地址寄存器
#define MPU6050_I2CSLV2_REG			0X2C	//IIC从机2数据地址寄存器
#define MPU6050_I2CSLV2_CTRL_REG	0X2D	//IIC从机2控制寄存器
#define MPU6050_I2CSLV3_ADDR_REG	0X2E	//IIC从机3器件地址寄存器
#define MPU6050_I2CSLV3_REG			0X2F	//IIC从机3数据地址寄存器
#define MPU6050_I2CSLV3_CTRL_REG	0X30	//IIC从机3控制寄存器
#define MPU6050_I2CSLV4_ADDR_REG	0X31	//IIC从机4器件地址寄存器
#define MPU6050_I2CSLV4_REG			0X32	//IIC从机4数据地址寄存器
#define MPU6050_I2CSLV4_DO_REG		0X33	//IIC从机4写数据寄存器
#define MPU6050_I2CSLV4_CTRL_REG	0X34	//IIC从机4控制寄存器
#define MPU6050_I2CSLV4_DI_REG		0X35	//IIC从机4读数据寄存器
#define MPU6050_I2CMST_STA_REG		0X36	//IIC主机状态寄存器
#define MPU6050_INTBP_CFG_REG		0X37	//中断/旁路设置寄存器
#define MPU6050_INT_EN_REG			0X38	//中断使能寄存器
#define MPU6050_INT_STA_REG			0X3A	//中断状态寄存器
#define MPU6050_ACCEL_XOUTH_REG		0X3B	//加速度值,X轴高8位寄存器
#define MPU6050_ACCEL_XOUTL_REG		0X3C	//加速度值,X轴低8位寄存器
#define MPU6050_ACCEL_YOUTH_REG		0X3D	//加速度值,Y轴高8位寄存器
#define MPU6050_ACCEL_YOUTL_REG		0X3E	//加速度值,Y轴低8位寄存器
#define MPU6050_ACCEL_ZOUTH_REG		0X3F	//加速度值,Z轴高8位寄存器
#define MPU6050_ACCEL_ZOUTL_REG		0X40	//加速度值,Z轴低8位寄存器
#define MPU6050_TEMP_OUTH_REG		0X41	//温度值高八位寄存器
#define MPU6050_TEMP_OUTL_REG		0X42	//温度值低8位寄存器
#define MPU6050_GYRO_XOUTH_REG		0X43	//陀螺仪值,X轴高8位寄存器
#define MPU6050_GYRO_XOUTL_REG		0X44	//陀螺仪值,X轴低8位寄存器
#define MPU6050_GYRO_YOUTH_REG		0X45	//陀螺仪值,Y轴高8位寄存器
#define MPU6050_GYRO_YOUTL_REG		0X46	//陀螺仪值,Y轴低8位寄存器
#define MPU6050_GYRO_ZOUTH_REG		0X47	//陀螺仪值,Z轴高8位寄存器
#define MPU6050_GYRO_ZOUTL_REG		0X48	//陀螺仪值,Z轴低8位寄存器
#define MPU6050_I2CSLV0_DO_REG		0X63	//IIC从机0数据寄存器
#define MPU6050_I2CSLV1_DO_REG		0X64	//IIC从机1数据寄存器
#define MPU6050_I2CSLV2_DO_REG		0X65	//IIC从机2数据寄存器
#define MPU6050_I2CSLV3_DO_REG		0X66	//IIC从机3数据寄存器
#define MPU6050_I2CMST_DELAY_REG	0X67	//IIC主机延时管理寄存器
#define MPU6050_SIGPATH_RST_REG		0X68	//信号通道复位寄存器
#define MPU6050_MDETECT_CTRL_REG	0X69	//运动检测控制寄存器
#define MPU6050_USER_CTRL_REG		0X6A	//用户控制寄存器
#define MPU6050_PWR_MGMT1_REG		0X6B	//电源管理寄存器1
#define MPU6050_PWR_MGMT2_REG		0X6C	//电源管理寄存器2
#define MPU6050_FIFO_CNTH_REG		0X72	//FIFO计数寄存器高八位
#define MPU6050_FIFO_CNTL_REG		0X73	//FIFO计数寄存器低八位
#define MPU6050_FIFO_RW_REG			0X74	//FIFO读写寄存器
#define MPU6050_DEVICE_ID_REG		0X75	//器件ID寄存器

//加速度量程
#define  AX_ACC_RANGE_2G  0  //2g	 
#define  AX_ACC_RANGE_4G  1  //4g
#define  AX_ACC_RANGE_8G  2  //8g	 
#define  AX_ACC_RANGE_16G 3  //16g

//陀螺仪量程
#define  AX_GYRO_RANGE_250  0  //250度/秒	 
#define  AX_GYRO_RANGE_500  1  //500度/秒
#define  AX_GYRO_RANGE_1000 2  //1000度/秒	 
#define  AX_GYRO_RANGE_2000 3  //2000度/秒	 

//带宽
#define  AX_DLPF_ACC184_GYRO188 1 //加速度带宽184Hz 陀螺仪带宽188Hz
#define  AX_DLPF_ACC94_GYRO98   2 //加速度带宽94Hz 陀螺仪带宽98Hz
#define  AX_DLPF_ACC44_GYRO42   3 //加速度带宽44Hz 陀螺仪带宽42Hz
#define  AX_DLPF_ACC21_GYRO20   4 //加速度带宽21Hz 陀螺仪带宽20Hz
#define  AX_DLPF_ACC10_GYRO10   5 //加速度带宽10Hz 陀螺仪带宽10Hz
#define  AX_DLPF_ACC5_GYRO5     6 //加速度带宽5Hz 陀螺仪带宽5Hz 

/*** OpenCTR 接口函数  M-Sensor运动传感器操作函数 **********/
void AX_MPU6050_Init(void);    //MPU6050传感器初始化

//设置参数函数
void AX_MPU6050_SetAccRange(uint8_t range);  //MPU6050设置加速度量程 
void AX_MPU6050_SetGyroRange(uint8_t range);  //MPU6050设置陀螺仪量程
void AX_MPU6050_SetGyroSmplRate(uint16_t smplrate);  //MPU6050设置陀螺仪采样率
void AX_MPU6050_SetDLPF(uint8_t bandwidth);  //MPU6050设置低通滤波器带宽

//获取数据
void AX_MPU6050_GetAccData(int16_t *pbuf);  //MPU6050获取加速度寄存器输出值
void AX_MPU6050_GetGyroData(int16_t *pbuf); //MPU6050获取陀螺仪寄存器输出值

#endif

/******************* (C) 版权 2023 XTARK **************************************/
