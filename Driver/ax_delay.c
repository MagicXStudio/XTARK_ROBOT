/**			                                                    
		   ____                    _____ _______ _____       @塔克创新
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   机器人控制器
									 
  ****************************************************************************** 
  *           
  * 版权所有： @塔克创新  版权所有，盗版必究
  * 公司网站： www.xtark.cn   www.tarkbot.com
  * 淘宝店铺： https://xtark.taobao.com  
  * 塔克微信： 塔克创新（关注公众号，获取最新更新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  软件延时函数
  ******************************************************************************
  * @说  明
  *
  * 1.延时函数使用滴答时钟实现
  * 
  ******************************************************************************
  */

#include "ax_delay.h"

#include "FreeRTOS.h"	//FreeRTOS使用		  

static u8  fac_us=0;	//us延时倍乘数	

/**
  * @简  述  延时函数初始化
  * @参  数  无
  * @返回值  无
  */
void AX_DELAY_Init(void) 
{	
	u32 reload;
	
	//延时函数SysTick配置
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); 	//时钟固定为AHB时钟
	
	fac_us=SystemCoreClock/1000000;				//不论是否使用OS,fac_us都需要使用
	reload=SystemCoreClock/1000000;				//每秒钟的计数次数 单位为M 
	
	reload*=1000000/configTICK_RATE_HZ;			//根据configTICK_RATE_HZ设定溢出时间
												                  //reload为24位寄存器,最大值:16777216,在72M下,约合0.233s左右	
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;  //开启SYSTICK中断
	SysTick->LOAD=reload; //每1/configTICK_RATE_HZ秒中断一次 	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //开启SYSTICK   	
}

/**
  * @简  述  软件微妙延时
  * @参  数  us：延时长度，单位us	  
  * @返回值  无
  */
void AX_Delayus(uint32_t us)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	 
	
	ticks=us*fac_us; 						  //需要的节拍数 
	told=SysTick->VAL;        		//刚进入时的计数器值
	
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;	 //这里注意一下SYSTICK是一个递减的计数器就可以了.
			else 
				tcnt+=reload-tnow+told;	  
			
			told=tnow;
			
			if(tcnt>=ticks)
				break;			    //时间超过/等于要延迟的时间,则退出.
		}  
	}										       				
}

/**
  * @简  述  软件ms延时,系统调度起来后，不要使用该函数
  * @参  数  ms：延时长度，单位ms	  
  * @返回值  无
  */
void AX_Delayms(uint16_t ms)
{
	AX_Delayus((uint32_t)(ms*1000));
	
}


/******************* (C) 版权 2023 XTARK **************************************/
