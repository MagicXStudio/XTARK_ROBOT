/**			                                                        
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  LED灯控制
  ******************************************************************************
  * @说  明
  *
  * 
  ******************************************************************************
  */

#include "ax_led.h"

/**
  * @简  述  LED 初始化
  * @参  数  无
  * @返回值  无
  */
void AX_LED_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//GPIO配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);	 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//关闭LED灯
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);	
}

    
