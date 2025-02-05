/**			                                                    
  * @作  者  Musk Han@XTARK
  * @内  容  调试串口通信
  * 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_UART1_H
#define __AX_UART1_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

//接口函数
void AX_UART1_Init(uint32_t baud);  //UART 串口初始化
void AX_UART1_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num);  //UART 发送数据（X-Protocol协议）


#endif 

    
