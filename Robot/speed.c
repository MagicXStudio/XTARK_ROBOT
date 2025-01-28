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
  * @内  容  机器人控制主函数
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "speed.h"
#include "robot.h"


/**
  * @简  述  电机PID控制函数
  * @参  数  spd_target:编码器速度目标值 ,范围（±250）
  *          spd_current: 编码器速度当前值
  * @返回值  电机PWM速度
  */
int16_t Motor_SpeedCtlA(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias,bias_last;

	//获得偏差值
	bias = spd_target - spd_current;
	
	//PID计算电机输出PWM值
	motor_pwm_out += ax_motor_kp*bias + ax_motor_kd*(bias-bias_last);
	
	//记录上次偏差
	bias_last = bias;
	
	//限制最大输出
	if(motor_pwm_out > 7200)
		motor_pwm_out = 7200;
	if(motor_pwm_out < -7200)
		motor_pwm_out = -7200;
	
	//返回PWM控制值
	return motor_pwm_out;
}	

/**
  * @简  述  电机PID控制函数
  * @参  数  spd_target:编码器速度目标值 
  *          spd_target: 编码器速度当前值
  * @返回值  电机PWM速度
  */
int16_t Motor_SpeedCtlB(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias,bias_last;

	//获得偏差值
	bias = spd_target - spd_current;
	
	//PID计算电机输出PWM值
	motor_pwm_out += ax_motor_kp*bias + ax_motor_kd*(bias-bias_last);
	
	//记录上次偏差
	bias_last = bias;
	
	//限制最大输出
	if(motor_pwm_out > 7200)
		motor_pwm_out = 7200;
	if(motor_pwm_out < -7200)
		motor_pwm_out = -7200;
	
	//返回PWM控制值
	return motor_pwm_out;
}

/**
  * @简  述  电机PID控制函数
  * @参  数  spd_target:编码器速度目标值 
  *          spd_target: 编码器速度当前值
  * @返回值  电机PWM速度
  */
int16_t Motor_SpeedCtlC(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias,bias_last;

	//获得偏差值
	bias = spd_target - spd_current;
	
	//PID计算电机输出PWM值
	motor_pwm_out += ax_motor_kp*bias + ax_motor_kd*(bias-bias_last);
	
	//记录上次偏差
	bias_last = bias;
	
	//限制最大输出
	if(motor_pwm_out > 7200)
		motor_pwm_out = 7200;
	if(motor_pwm_out < -7200)
		motor_pwm_out = -7200;
	
	//返回PWM控制值
	return motor_pwm_out;
}

/**
  * @简  述  电机PID控制函数
  * @参  数  spd_target:编码器速度目标值 
  *          spd_target: 编码器速度当前值
  * @返回值  电机PWM速度
  */
int16_t Motor_SpeedCtlD(float spd_target, float spd_current)
{
	static int16_t motor_pwm_out;
	static float bias,bias_last;

	//获得偏差值
	bias = spd_target - spd_current;
	
	//PID计算电机输出PWM值
	motor_pwm_out += ax_motor_kp*bias + ax_motor_kd*(bias-bias_last);
	
	//记录上次偏差
	bias_last = bias;
	
	//限制最大输出
	if(motor_pwm_out > 7200)
		motor_pwm_out = 7200;
	if(motor_pwm_out < -7200)
		motor_pwm_out = -7200;
	
	//返回PWM控制值
	return motor_pwm_out;
}


/******************* (C) 版权 2019 XTARK **************************************/

