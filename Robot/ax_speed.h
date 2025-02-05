/**			                                                    
     
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  机器人轮子PID速度控制
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_SPEED_H
#define __AX_SPEED_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//电机PID闭环速度控制函数
int16_t AX_SPEED_PidCtlA(float spd_target, float spd_current);   //PID控制函数，电机A
int16_t AX_SPEED_PidCtlB(float spd_target, float spd_current);    //PID控制函数，电机B


#endif

    
