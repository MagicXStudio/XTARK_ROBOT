/**			                                                    
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  FLASH读写
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_FLASH_H
#define __AX_FLASH_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//OpenCTR驱动接口函数
void AX_FLASH_Erase(void);   //擦除整个选定的扇区
void AX_FLASH_Read(uint16_t addr, uint16_t *pbuff, uint8_t num);   //从指定地址开始写入指定长度的数据
void AX_FLASH_Write(uint16_t addr, uint16_t *pbuff, uint8_t num);       //从指定地址开始读出指定长度的数据


#endif 

    
