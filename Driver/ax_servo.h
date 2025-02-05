/**			                                                        
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V2.0
  * @日  期  2022-7-26
  * @内  容  PWM接口舵机控制
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_SERVO_H
#define __AX_SERVO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

//接口函数
void AX_SERVO_S1234_Init(void);                //舵机接口初始化
void AX_SERVO_S1_SetAngle(int16_t angle);      //舵机控制   
void AX_SERVO_S2_SetAngle(int16_t angle);      //舵机控制
void AX_SERVO_S3_SetAngle(int16_t angle);      //舵机控制   
void AX_SERVO_S4_SetAngle(int16_t angle);      //舵机控制

void AX_SERVO_S56_Init(void);                  //舵机接口初始化
void AX_SERVO_S5_SetAngle(int16_t angle);      //舵机控制   
void AX_SERVO_S6_SetAngle(int16_t angle);      //舵机控制

#endif

    
