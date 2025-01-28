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
  * @版  本  V2.0
  * @日  期  2022-7-26
  * @内  容  PWM接口舵机控制
  *
  ******************************************************************************
  * @说  明
  *
  ******************************************************************************
  */

#include "ax_servo.h" 

/**
  * @简  述  舵机接口初始化	
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S1234_Init(void)
{ 
	
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure; 
	
	//定时器通道IO配置
	//GPIO及复用功能时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);	

	//配置IO口为复用功能-定时器通道
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//TIM时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	//定时器配置	PWM工作模式，频率50Hz,周期20ms  
	//占空比调节范围：0-1.5ms-2.5ms 0-1500-2500	初始化为1500
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;  //定时器分频，分频后的频率为1M
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=20000-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	//PWM1 Mode configuration
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;	    //占空比初始化，90度
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

	//PWM1 Mode configuration
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	//PWM1 Mode configuration
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	//PWM1 Mode configuration
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM8, ENABLE);

	//使能定时器
	TIM_Cmd(TIM8, ENABLE);	
	
	//使能MOE位
	TIM_CtrlPWMOutputs(TIM8,ENABLE);		
	
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S1_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare4(TIM8,(1.111f*angle + 1500));
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S2_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare3(TIM8,(1.111f*angle + 1500));
	
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S3_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare2(TIM8,(1.111f*angle + 1500));
	
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S4_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare1(TIM8,(1.111f*angle + 1500));
	
}

/**
  * @简  述  舵机接口初始化	
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S56_Init(void)
{ 
	
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure; 	
	
	//GPIO功能时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);


	//配置IO口为复用功能-定时器通道
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//TIM时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	//定时器配置	PWM工作模式，频率50Hz,周期20ms  占空比调节范围：0-1.5ms-2.5ms 0-1500-2500	初始化为1500
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;  //定时器分频，分频后的频率为1M
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=20000-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	//PWM1 Mode configuration: Channel
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1500;	    //占空比初始化，90度
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);

	//PWM1 Mode configuration: Channel
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM5, ENABLE);

	//使能定时器
	TIM_Cmd(TIM5, ENABLE);		
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S5_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare1(TIM5,(1.111f*angle + 1500));
	
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S6_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare2(TIM5,(1.111f*angle + 1500));
	
}

/******************* (C) 版权 2023 XTARK **************************************/
