/**			                                                        
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  BEEP蜂鸣器控制
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_BEEP_H
#define __AX_BEEP_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//接口函数
void AX_BEEP_Init(void);

//蜂鸣器操作函数宏定义
#define AX_BEEP_On()  	     GPIO_SetBits(GPIOC, GPIO_Pin_13)      //蜂鸣器鸣叫
#define AX_BEEP_Off()		 GPIO_ResetBits(GPIOC, GPIO_Pin_13)    //蜂鸣器关闭
#define AX_BEEP_Toggle()     GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction) (1 - GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)))	//蜂鸣器状态翻转

#endif 

    
