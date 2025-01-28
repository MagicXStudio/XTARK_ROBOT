/**			                                                    
		   ____                    _____ _____  _____        XTARK@塔克创新
		  / __ \                  / ____|  __ \|  __ \  
		 | |  | |_ __   ___ _ __ | |    | |__) | |__) |
		 | |  | | '_ \ / _ \ '_ \| |    |  _  /|  ___/ 
		 | |__| | |_) |  __/ | | | |____| | \ \| |     
		  \____/| .__/ \___|_| |_|\_____|_|  \_\_|     
		    		| |                                    
		    		|_|  OpenCRP 树莓派 专用ROS机器人控制器                                   
									 
  ****************************************************************************** 
  *           
  * 版权所有： XTARK@塔克创新  版权所有，盗版必究
  * 官网网站： www.xtark.cn
  * 淘宝店铺： https://shop246676508.taobao.com  
  * 塔克媒体： www.cnblogs.com/xtark（博客）
	* 塔克微信： 微信公众号：塔克创新（获取最新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @日  期  2019-7-26
  * @内  容  VIN输入电压检测
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ROBOT_H
#define __ROBOT_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//C库函数的相关头文件
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>

//FreeRTOS头文件
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

//外设相关头文件
#include "ax_sys.h"    //系统设置
#include "ax_delay.h"  //软件延时
#include "ax_led.h"    //LED灯控制
#include "ax_beep.h"   //蜂鸣器控制
#include "ax_vin.h"    //输入电压检测
#include "ax_key.h"    //按键检测

#include "ax_uart1.h"    //调试串口
#include "ax_uart2.h"    //蓝牙串口
#include "ax_uart4.h"    //TTL串口
#include "ax_uart5.h"    //预留串口

#include "ax_motor.h"    //直流电机调速控制
#include "ax_encoder.h"  //编码器控制
#include "ax_servo.h"    //舵机控制

#include "ax_mpu6050.h"  //IMU加速度陀螺仪

#include "ax_rgb.h"      //RGB彩灯控制
#include "ax_sbus.h"     //SBUS航模遥控器控制
#include "ax_oled.h"     //OLED显示


//电机速度结构体
typedef struct  
{
	float  Wheel_RT;       //车轮实时速度，单位m/s
	float  Wheel_TG;       //车轮目标速度，单位m/s
	short  Motor_Pwm;      //电机PWM数值
}MOTOR_Data;

//机器人速度结构体
typedef struct  
{
	short  I_X;     //X轴速度（16位整数）
	short  I_Y;     //Y轴速度（16位整数）
	short  I_W;     //Yaw旋转轴速度（16位整数）
	
	float  F_X;     //X轴速度，浮点
	float  F_Y;     //Y轴速度，浮点
	float  F_W;     //Yaw旋转轴速度，浮点
	
}ROBOT_Velocity;

//机器人转向控制结构体
typedef struct  
{
	float  Radius;     //转弯半径
	float  Angle;     //机器人转向角度
	float  RAngle;     //前右轮转向角度
	float  SAngle;     //舵机角度
	
}ROBOT_Steering;

//机器人IMU数据
typedef struct  
{
	short  ACC_X;     //X轴
	short  ACC_Y;     //Y轴
	short  ACC_Z;     //Z轴
	
	short  GYRO_X;     //X轴
	short  GYRO_Y;     //Y轴
	short  GYRO_Z;     //Z轴
	
}ROBOT_IMU;


#define  PI   3.1416     //圆周率PI

//电池
#define  VBAT_40P    1065     //电池40%电压
#define  VBAT_20P    1012     //电池20%电压
#define  VBAT_10P    984      //电池10%电压

#define  PID_RATE        50        //PID频率

//机器人参数
#define  R_WHEEL_BASE   0.165	  //轮距，左右轮的距离
#define  R_ACLE_BASE    0.160     //轴距，前后轮的距离


#define  R_TURN_R_MINI  0.35      //最小转弯半径( L*cot30-W/2)

//轮子参数 
#define  WHEEL_DIAMETER	     0.0725		//轮子直径
#define  WHEEL_RESOLUTION    1440.0    //编码器分辨率

//轮子速度m/s与编码器转换系数
#define  WHEEL_SCALE  (PI*WHEEL_DIAMETER*PID_RATE/WHEEL_RESOLUTION) 

//机器人速度限制
#define R_VX_LIMIT  1500   //X轴速度限值 m/s*1000
#define R_VY_LIMIT  1200   //Y轴速度限值 m/s*1000
#define R_VW_LIMIT  6280   //W旋转角速度限值 rad/s*1000

//机器人通信帧头定义
//机器人通信帧头定义
#define  ID_CPR2ROS_DATA    0x10    //下位计向ROS发送的综合数据
#define  ID_ROS2CRP_VEL     0x50    //ROS向下位机发送的速度数据
#define  ID_ROS2CRP_IMU     0x51    //ROS向下位机发送的IMU陀螺仪零偏校准指令
#define  ID_ROS2CRP_AKM     0x5f    //ROS向下位机发送的AKM机器人舵机零偏补偿角，其它类型忽略

//全局变量
extern int16_t ax_imu_acc_data[3];  
extern int16_t ax_imu_gyro_data[3];
extern int16_t ax_imu_gyro_offset[3];   

extern int16_t ax_motor_kp;  
extern int16_t ax_motor_ki;    
extern int16_t ax_motor_kd; 

extern int16_t ax_servo_offset;

extern int8_t ax_imu_calibrate_flag;

//电机数据结构体
extern MOTOR_Data MOTOR_A,MOTOR_B;

//机器人速度数据结构体
extern ROBOT_Velocity  RobotV_RT,RobotV_TG;


//任务句柄
extern TaskHandle_t Robot_Task_Handle;
extern TaskHandle_t Key_Task_Handle;
extern TaskHandle_t Bat_Task_Handle;


#endif

/******************* (C) 版权 2019 XTARK **************************************/
