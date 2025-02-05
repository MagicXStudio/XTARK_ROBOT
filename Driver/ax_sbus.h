/**			                                                         
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V2.0
  * @日  期  2022-7-26
  * @内  容  SBUS航模遥控器驱动程序 
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_SBUS_H
#define __AX_SBUS_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//OpenCTR驱动接口函数
void AX_SBUS_Init(void);  //UART 调试串口初始化
uint8_t AX_SBUS_GetRxData(uint16_t *pbuf);  //UART 获取遥控器接收的数据


#endif 

    
