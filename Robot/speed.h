/**			                                                         
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @日  期  2019-7-26
  * @内  容  VIN输入电压检测
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPEED_H
#define __SPEED_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//电机PID闭环速度控制函数
int16_t Motor_SpeedCtlA(float spd_target, float spd_current);   //PID控制函数，电机A
int16_t Motor_SpeedCtlB(float spd_target, float spd_current);    //PID控制函数，电机B
int16_t Motor_SpeedCtlC(float spd_target, float spd_current);    //PID控制函数，电机C
int16_t Motor_SpeedCtlD(float spd_target, float spd_current);    //PID控制函数，电机D


#endif

/******************* (C) 版权 2019 XTARK **************************************/
