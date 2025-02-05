/**			                                                        
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  OLED显示驱动程序
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_OLED_H
#define __AX_OLED_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "ax_delay.h"

/***  OLED操作函数 **********/
void AX_OLED_Init(void);    //OLED初始化
void AX_OLED_ClearScreen(void);    //OLED清除屏幕

void AX_OLED_DispChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t mode);    // OLED指定位置显示一个ASCII字符（6X8）
void AX_OLED_DispStr(uint8_t x, uint8_t y, uint8_t *ch, uint8_t mode);    //OLED指定位置显示ASCII字符串（6X8）
void AX_OLED_DispNum(uint8_t x, uint8_t y, uint8_t num, uint8_t mode);    //OLED指定位置显示一个数字（6X8）
void AX_OLED_DispValue(uint8_t x, uint8_t y, int32_t value, uint8_t inte, uint8_t deci, uint8_t mode);    //OLED指定位置显示数值

void AX_OLED_Disp16Char(uint8_t x, uint8_t y, uint8_t ch, uint8_t mode);    // OLED指定位置显示一个ASCII字符（8X16）
void AX_OLED_Disp16Str(uint8_t x, uint8_t y, uint8_t *ch, uint8_t mode);    //OLED指定位置显示ASCII字符串（8X16）
void AX_OLED_Disp16Num(uint8_t x, uint8_t y, uint8_t num, uint8_t mode);    //OLED指定位置显示一个数字（8X16）
void AX_OLED_Disp16Value(uint8_t x, uint8_t y, int32_t value, uint8_t inte, uint8_t deci, uint8_t mode);    //OLED指定位置显示数值

void AX_OLED_DispPicture(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, const uint8_t *pbuf, uint8_t mode);    //OLED指定位置显示一个指定尺寸照片
void AX_OLED_DispChinese(uint8_t x, uint8_t y, const uint8_t *pbuf, uint8_t mode);    //OLED指定位置显示一个汉字（16X16）

#endif

    
