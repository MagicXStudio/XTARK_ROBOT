/**			                                                    
		   ____                    _____ _______ _____       XTARK@塔克创新
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   机器人控制器
									 
  ****************************************************************************** 
  *           
  * 版权所有： XTARK@塔克创新  版权所有，盗版必究
  * 公司网站： www.xtark.cn   www.tarkbot.com
  * 淘宝店铺： https://xtark.taobao.com  
  * 塔克微信： 塔克创新（关注公众号，获取最新更新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @内  容  PS2无线手柄函数文件
  *
  ******************************************************************************
  * @说  明
  *
  *   PS2数据定义
  *   BYTE   DATA   解释
  *   01     idle
  *   02     0x73   手柄工作模式
  *   03     0x5A   Bit0  Bit1  Bit2  Bit3  Bit4  Bit5  Bit6  Bit7
  *   04     data   SLCT  JOYR  JOYL  STRT   UP   RGIHT  DOWN   L
  *   05     data   L2     R2     L1    R1   Y     B     A      X
  *   06     data   右边摇杆  0x00 = 左    0xff = 右
  *   07     data   右边摇杆  0x00 = 上    0xff = 下
  *   08     data   左边摇杆  0x00 = 左    0xff = 右
  *   09     data   左边摇杆  0x00 = 上    0xff = 下
  * 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_PS2_H
#define __AX_PS2_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//PS2按键定义
#define  PS2_BT1_SELECT     0x01    //选择按键
#define  PS2_BT1_JOY_L      0x02    //左摇杆按键
#define  PS2_BT1_JOY_R      0x04    //右摇杆按键 
#define  PS2_BT1_START      0x08    //启动按键
#define  PS2_BT1_UP         0x10    //方向 上按键
#define  PS2_BT1_RIGHT      0x20    //方向 右按键
#define  PS2_BT1_DOWN       0x40    //方向 下按键 
#define  PS2_BT1_LEFT       0x80    //方向 左按键

#define  PS2_BT2_L2         0x01    //选择按键
#define  PS2_BT2_R2         0x02    //左摇杆按键
#define  PS2_BT2_L1         0x04    //右摇杆按键 
#define  PS2_BT2_R1         0x08    //启动按键
#define  PS2_BT2_Y          0x10    //功能 Y按键(或三角) 
#define  PS2_BT2_B          0x20    //功能 B按键(圆形)
#define  PS2_BT2_A          0x40    //功能 A按键(叉号)
#define  PS2_BT2_X          0x80    //功能 X按键(方形)

//PS2手柄键值数据结构体	 
typedef struct			 				
{
  uint8_t mode;		    /* 手柄的工作模式 */

  uint8_t btn1;         /* B0:SLCT B1:JR  B0:JL B3:STRT B4:UP B5:R B6:DOWN  B7:L   */

  uint8_t btn2;         /* B0:L2   B1:R2  B2:L1 B3:R1   B4:Y  B5:B B6:A     B7:X */

  uint8_t RJoy_LR;      /* 右边摇杆  0x00 = 左    0xff = 右   */

  uint8_t RJoy_UD;      /* 右边摇杆  0x00 = 上    0xff = 下   */

  uint8_t LJoy_LR;      /* 左边摇杆  0x00 = 左    0xff = 右   */

  uint8_t LJoy_UD;      /* 左边摇杆  0x00 = 上    0xff = 下   */
	
}JOYSTICK_TypeDef;


/*** PS2无线手柄操作函数 **********/
void AX_PS2_Init(void);  //PS2初始化
void AX_PS2_ScanKey(JOYSTICK_TypeDef* JoystickStruct);//PS2获取按键及摇杆数值

#endif 

/******************* (C) 版权 2023 XTARK **************************************/
