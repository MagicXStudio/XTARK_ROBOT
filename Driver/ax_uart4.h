/**			                                                    
  * @作  者  Musk Han@XTARK
  * @内  容  TTL串口通信
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_UART4_H
#define __AX_UART4_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//OpenCTR接口函数
void    AX_UART4_Init(uint32_t baud);  //扩展串口初始化
uint8_t AX_UART4_GetData(uint8_t *pbuf);
void    AX_UART4_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num);  //发送数据（X-Protocol协议）

#endif 

    
