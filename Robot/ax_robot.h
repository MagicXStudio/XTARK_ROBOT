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
  * @内  容  机器人控制主函数
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_ROBOT_H
#define __AX_ROBOT_H

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
#include "ax_sys.h"      //系统设置
#include "ax_delay.h"    //软件延时
#include "ax_led.h"      //LED灯控制
#include "ax_beep.h"     //蜂鸣器控制
#include "ax_vin.h"      //输入电压检测
#include "ax_key.h"      //按键检测 
#include "ax_flash.h"    //FLASH读写操作

#include "ax_uart1.h"    //调试串口
#include "ax_uart2.h"    //蓝牙串口
#include "ax_uart4.h"    //TTL串口
#include "ax_uart5.h"    //预留串口

#include "ax_servo.h"    //舵机控制
#include "ax_motor.h"    //直流电机调速控制
#include "ax_encoder.h"  //编码器控制

#include "ax_mpu6050.h"  //IMU加速度陀螺仪测量

#include "ax_rgb.h"      //RGB彩灯控制
#include "ax_sbus.h"     //SBUS航模遥控器控制
#include "ax_oled.h"     //OLED显示
#include "ax_ps2.h"      //PS2手柄

#include "ax_akm.h"      //阿克曼机器人处理函数


//机器人轮子速度数据结构体
typedef struct  
{
	double  RT;       //车轮实时速度，单位m/s
	float  TG;       //车轮目标速度，单位m/s
	short  PWM;      //车轮PWM控制速度
	
}ROBOT_Wheel;

//机器人速度结构体
typedef struct  
{
	short  RT_IX;     //实时X轴速度（16位整数）
	short  RT_IY;     //实时Y轴速度（16位整数）
	short  RT_IW;     //实时Yaw旋转轴速度（16位整数）
	
	short  TG_IX;     //目标X轴速度（16位整数）
	short  TG_IY;     //目标Y轴速度（16位整数）
	short  TG_IW;     //目标Yaw旋转轴速度（16位整数）
	
	float  RT_FX;     //实时X轴速度（浮点）
	float  RT_FY;     //实时Y轴速度（浮点）
	float  RT_FW;     //实时Yaw旋转轴速度（浮点）
	
	float  TG_FX;     //目标X轴速度（浮点）
	float  TG_FY;     //目标Y轴速度（浮点）
	float  TG_FW;     //目标Yaw旋转轴速度（浮点）
	
}ROBOT_Velocity;


//机器人IMU数据
typedef struct  
{
	short  ACC_X;     //X轴
	short  ACC_Y;     //Y轴
	short  ACC_Z;     //Z轴
	
	short  GYRO_X;     //X轴
	short  GYRO_Y;     //Y轴
	short  GYRO_Z;     //Z轴
	
}ROBOT_Imu;


//RGB灯效数据
typedef struct  
{
	uint8_t  M;    //灯效主模式
	uint8_t  S;    //灯效从模式
	uint8_t  T;    //灯效时间参数
	
	uint8_t  R;     //灯效颜色 R
	uint8_t  G;     //灯效颜色 G
	uint8_t  B;     //灯效颜色 B
	
}ROBOT_Light;

//杂类
#define  PI           3.1416     //圆周率PI
#define  SQRT3        1.732      //3开平方
#define  PID_RATE     50         //PID频率

//电池
#define  VBAT_40P    1065     //电池40%电压
#define  VBAT_20P    1012     //电池20%电压
#define  VBAT_10P    984      //电池10%电压

//机器人软件版本
#define  ROBOT_FW_VER   "V2.00"

/******机器人类型***********************************/

//机器人类型定义
#define ROBOT_AKM   0x01   //麦克纳姆轮底盘
#define ROBOT_TWD1  0x02   //两轮差速转向底盘1

//机器人定义类型
#define ROBOT_TYPE   ROBOT_AKM


/******机器人参数*************************************/

//阿克曼机器人参数
#define  AKM_WHEEL_BASE           0.165	     //轮距，左右轮的距离
#define  AKM_ACLE_BASE            0.156f     //轴距，前后轮的距离
#define  AKM_WHEEL_DIAMETER	      0.075		 //轮子直径
#define  AKM_WHEEL_RESOLUTION     1560.0     //编码器分辨率(13线),减速比30,13x30x4=1560
#define  AKM_TURN_R_MINI          0.35f      //最小转弯半径( L*cot30-W/2)
#define  AKM_WHEEL_SCALE          (PI*AKM_WHEEL_DIAMETER*PID_RATE/AKM_WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数

//两轮差速机器人参数 
#define  TWD1_WHEEL_DIAMETER	   0.075	 //轮子直径
#define  TWD1_WHEEL_BASE           0.172	 //轮距，左右轮的距离
#define  TWD1_WHEEL_RESOLUTION     1560.0    //编码器分辨率(13线),减速比30,13x30x4=1560
#define  TWD1_WHEEL_SCALE          (PI*TWD1_WHEEL_DIAMETER*PID_RATE/TWD1_WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数


/******机器人通信协议*************************************/

//串口通信帧头定义
#define  ID_UTX_DATA    0x10    //发送的综合数据
#define  ID_URX_VEL     0x50    //接收的速度数据
#define  ID_URX_IMU     0x51    //接收的IMU陀螺仪零偏校准指令
#define  ID_URX_LG      0x52    //接收的灯光控制指令
#define  ID_URX_LS      0x53    //接收的灯光效果保存指令
#define  ID_URX_BP      0x54    //接收蜂鸣器鸣叫指令
#define  ID_URX_RTY     0x5A    //接收的机器人型号选择

//蓝牙APP通信帧头定义
#define  ID_BLERX_CM      0x30    //APP蓝牙发送 蓝牙连接指令
#define  ID_BLERX_YG      0x31    //APP蓝牙发送 摇杆模式 控制指令
#define  ID_BLERX_SB      0x32    //APP蓝牙发送 手柄模式 控制指令
#define  ID_BLERX_ZL      0x33    //APP蓝牙发送 重力模式 控制指令
#define  ID_BLERX_TK      0x34    //APP蓝牙发送 坦克模式 控制指令
#define  ID_BLERX_AM      0x3A    //APP蓝牙发送 机械臂模式 控制指令
#define  ID_BLERX_LG      0x41    //APP蓝牙发送 灯光控制指令
#define  ID_BLERX_LS      0x42    //APP蓝牙发送 保存灯光效果指令

//机器人速度限制
#define R_VX_LIMIT  1500   //X轴速度限值 m/s*1000
#define R_VY_LIMIT  1200   //Y轴速度限值 m/s*1000
#define R_VW_LIMIT  6280   //W旋转角速度限值 rad/s*1000

//灯光模式
#define  LEFFECT1    0x01    //单色模式  
#define  LEFFECT2    0x02    //呼吸模式  
#define  LEFFECT3    0x03    //彩色模式  
#define  LEFFECT4    0x04    //警灯模式 
#define  LEFFECT5    0x05    //定义模式  
#define  LEFFECT6    0x06    //定义模式

//机器人控制方式
#define  CTL_ROS    0x00    //ROS控制，包含串口和CAN
#define  CTL_PS2    0x01    //PS2手柄控制
#define  CTL_APP    0x02    //APP控制
#define  CTL_RMS    0x03    //SBUS航模遥控器控制	

//蜂鸣器鸣长短
#define  BEEP_SHORT   0x01    //蜂鸣器短鸣叫一声(200ms)
#define  BEEP_LONG    0x02    //蜂鸣器长鸣叫一声(1000ms)

//机器人关键全局变量
extern  ROBOT_Velocity  R_Vel; //机器人速度数据
extern  ROBOT_Wheel  R_Wheel_A,R_Wheel_B,R_Wheel_C,R_Wheel_D; //机器人轮子数据
extern  ROBOT_Imu  R_Imu; //机器人IMU数据
extern  ROBOT_Light  R_Light; //机器人RGB数据
extern  uint16_t R_Bat_Vol;  //机器人电池电压数据

//全局变量
extern int16_t ax_imu_acc_data[3];  
extern int16_t ax_imu_gyro_data[3];
extern int16_t ax_imu_gyro_offset[3];   

extern int16_t ax_motor_kp;  
extern int16_t ax_motor_ki;    
extern int16_t ax_motor_kd; 

extern int8_t ax_imu_calibrate_flag;
extern int8_t ax_model_flag;
extern uint8_t ax_light_enable;
extern uint8_t ax_light_save_flag;
extern uint8_t ax_beep_ring;  
extern uint8_t ax_beep_enable;  
extern uint8_t ax_control_mode;

//FLASH参数存储变量
extern uint16_t ax_param[10];
extern uint8_t ax_param_flag;

//PS2手柄键值结构体
extern JOYSTICK_TypeDef my_joystick;  

//阿克曼机器人
extern int16_t ax_akm_offset;
extern int16_t ax_akm_angle;

//任务句柄
extern TaskHandle_t Robot_Task_Handle;
extern TaskHandle_t Key_Task_Handle;
extern TaskHandle_t Disp_Task_Handle;
extern TaskHandle_t Trivia_Task_Handle;


#endif

/******************* (C) 版权 2023 XTARK **************************************/
