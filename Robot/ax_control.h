/**			                                                    
    
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @内  容  机器人控制处理文件
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_CONTROL_H
#define __AX_CONTROL_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//机器人各种控制方式处理文件
void AX_CTL_Ps2(void);     //PS2手柄控制
void AX_CTL_App(void);     //APP控制
void AX_CTL_RemoteSbus(void);  //SBUS航模遥控器控制

#endif

    
