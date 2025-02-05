/**			                                                       
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V2.0
  * @日  期  2022-7-26
  * @内  容  WS2812 RGB灯带控制 
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_RGB_H
#define __AX_RGB_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//灯个数
#define  PIXEL_NUM  8

//接口函数
void AX_RGB_Init(void);
void AX_RGB_SetFullColor(uint8_t b, uint8_t r, uint8_t g);
void AX_RGB_SetPixelColor(uint8_t pixel[PIXEL_NUM][3]);
void AX_RGB_SetPixelColor1( const uint8_t pixel[PIXEL_NUM][3]);


#endif

    
