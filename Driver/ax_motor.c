/**			                                                    
		   ____                    _____ _______ _____       XTARK@塔克创新
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   机器人控制器
									 
  ****************************************************************************** 
  *           
  * 版权所有： XTARK@塔克创新  版权所有，盗版必究
  * 公司网站： www.xtark.cn   www.tarkbot.com
  * 淘宝店铺： https://xtark.taobao.com  
  * 塔克微信： 塔克创新（关注公众号，获取最新更新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARKXTARK
  * @内  容  电机PWM控制函数
  *
  ******************************************************************************
  * @说  明
  *
  ******************************************************************************
  */


#include "ax_motor.h" 

/**
  * @简  述  电机PWM控制初始化，PWM频率为20KHZ	
  * @参  数  无
  * @返回值  无
  */
void AX_MOTOR_Init(void)
{ 
	
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure; 

	//GPIO功能时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	//配置IO口为复用功能-定时器通道
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度100MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//TIM时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	//Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 3600-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	//PWM1 Mode configuration: Channel1 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;	    //占空比初始化
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	//PWM1 Mode configuration: Channel2
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	//PWM1 Mode configuration: Channel3
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	//PWM1 Mode configuration: Channel4
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM4, ENABLE);

	//TIM enable counter
	TIM_Cmd(TIM4, ENABLE);   

	//使能MOE位
	TIM_CtrlPWMOutputs(TIM4,ENABLE);	
}

/**
  * @简  述 电机PWM速度控制
  * @参  数 speed 电机转速数值，范围-3600~3600
  * @返回值 无
  */
void AX_MOTOR_A_SetSpeed(int16_t speed)
{
	int16_t temp;
	
	temp = speed;
	
	if(temp>3600)
		temp = 3600;
	if(temp<-3600)
		temp = -3600;
	
	if(temp > 0)
	{
		TIM_SetCompare1(TIM4, 3600);
		TIM_SetCompare2(TIM4, (3600 - temp));
	}
	else
	{
		TIM_SetCompare2(TIM4, 3600);
		TIM_SetCompare1(TIM4, (3600 + temp));
	}
}

/**
  * @简  述 电机PWM速度控制
  * @参  数 speed 电机转速数值，范围-3600~3600
  * @返回值 无
  */
void AX_MOTOR_B_SetSpeed(int16_t speed)
{
	int16_t temp;
	
	temp = speed;
	
	if(temp>3600)
		temp = 3600;
	if(temp<-3600)
		temp = -3600;
	
	if(temp > 0)
	{
		TIM_SetCompare3(TIM4, 3600);
		TIM_SetCompare4(TIM4, (3600 - temp));
	}
	else
	{
		TIM_SetCompare4(TIM4, 3600);
		TIM_SetCompare3(TIM4, (3600 + temp));
	}
}


/******************* (C) 版权 2023 XTARK **************************************/
