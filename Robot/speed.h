/**			                                                    
		   ____                    _____ _____  _____        XTARK@塔克创新
		  / __ \                  / ____|  __ \|  __ \  
		 | |  | |_ __   ___ _ __ | |    | |__) | |__) |
		 | |  | | '_ \ / _ \ '_ \| |    |  _  /|  ___/ 
		 | |__| | |_) |  __/ | | | |____| | \ \| |     
		  \____/| .__/ \___|_| |_|\_____|_|  \_\_|     
		    		| |                                    
		    		|_|  OpenCRP 树莓派 专用ROS机器人控制器                                   
									 
  ****************************************************************************** 
  *           
  * 版权所有： XTARK@塔克创新  版权所有，盗版必究
  * 官网网站： www.xtark.cn
  * 淘宝店铺： https://shop246676508.taobao.com  
  * 塔克媒体： www.cnblogs.com/xtark（博客）
	* 塔克微信： 微信公众号：塔克创新（获取最新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @日  期  2019-7-26
  * @内  容  VIN输入电压检测
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPEED_H
#define __SPEED_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f10x.h"

//电机PID闭环速度控制函数
int16_t Motor_SpeedCtlA(float spd_target, float spd_current);   //PID控制函数，电机A
int16_t Motor_SpeedCtlB(float spd_target, float spd_current);    //PID控制函数，电机B
int16_t Motor_SpeedCtlC(float spd_target, float spd_current);    //PID控制函数，电机C
int16_t Motor_SpeedCtlD(float spd_target, float spd_current);    //PID控制函数，电机D


#endif

/******************* (C) 版权 2019 XTARK **************************************/
