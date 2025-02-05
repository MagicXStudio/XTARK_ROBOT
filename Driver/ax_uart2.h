/**			                                                        
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  蓝牙串口通信
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_UART_PI_H
#define __AX_UART_PI_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//接口函数
void AX_UART2_Init(uint32_t baud);  //扩展串口初始化
uint8_t AX_UART2_GetData(uint8_t *pbuf);
void AX_UART2_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num);  //发送数据（X-Protocol协议）

#endif 

    
