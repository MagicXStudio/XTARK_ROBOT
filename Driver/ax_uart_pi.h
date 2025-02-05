/**			                                                        
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @日  期  2019-7-26
  * @内  容  PI串口通信
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_UART_PI_H
#define __AX_UART_PI_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//OpenCRP驱动接口函数
void AX_UART_PI_Init(uint32_t baud);  //扩展串口初始化
uint8_t AX_UART_PI_GetRxData(uint8_t *pbuf);
void AX_UART_PI_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num);  //发送数据（X-Protocol协议）

#endif 
