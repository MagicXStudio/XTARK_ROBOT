/**			                                                    
    
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  BEEP蜂鸣器控制
  ******************************************************************************
  * @说  明
  *
  * 
  ******************************************************************************
  */

#include "ax_beep.h"

/**
  * @简  述  BEEP 初始化
  * @参  数  无
  * @返回值  无
  */
void AX_BEEP_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//GPIO配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	//关闭蜂鸣器
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	
}	


    
