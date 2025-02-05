/**			                                                    
    
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  机器人运动学解析
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_kinematics.h"
#include "ax_robot.h"
#include "ax_speed.h"


//根据机器人类型，选择编译机器人运动学处理函数


#if (ROBOT_TYPE == ROBOT_AKM)
/**
  * @简  述  机器人运动学处理-阿克曼
  * @参  数  无
  * @返回值  无
  */
void AX_ROBOT_Kinematics(void)
{

	float akm_spd;  //速度
    float akm_angle; //前轮转向角度
	float akm_angle_r;  //右前轮转向角度
	float akm_angle_s;  //舵机转向角度
	float radius;  //转弯半径
	int16_t servo_angle; //舵机控制角度
	
	//通过编码器获取车轮实时转速m/s
	R_Wheel_A.RT = (float) ((int16_t)AX_ENCODER_A_GetCounter()*AKM_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float)-((int16_t)AX_ENCODER_B_GetCounter()*AKM_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);	
	
	//调试输出轮子转速
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	/*******正向运动学解析(根据前进速度和舵机转角计算)*********************************/
	
	//机器人目标速度限制
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	
	//转向角度限制
//	if( ax_akm_angle > R_VX_LIMIT )    ax_akm_angle = R_VX_LIMIT;
//	if( ax_akm_angle < (-R_VX_LIMIT))  ax_akm_angle = (-R_VX_LIMIT);
	
	//转换为浮点数计算
	akm_spd = R_Vel.TG_IX/1000.0;
	akm_angle = ax_akm_angle/1000.0;
	
	//根据转向角度计算转弯半径
	radius = AKM_ACLE_BASE/tan(akm_angle);		
	
	//根据前轮角度计算右前轮角度
	if(ax_akm_angle !=0 )
	{

		//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
		R_Wheel_A.TG = akm_spd*(radius-0.5*AKM_WHEEL_BASE)/radius;
		R_Wheel_B.TG = akm_spd*(radius+0.5*AKM_WHEEL_BASE)/radius;					
		
		//计算右前轮角度
		akm_angle_r = (atan(AKM_ACLE_BASE/((AKM_ACLE_BASE/tan(akm_angle*0.01745))+0.5*AKM_WHEEL_BASE)))*(180.0/PI);
	}
	else
	{
		//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
		R_Wheel_A.TG = akm_spd;
		R_Wheel_B.TG = akm_spd;	
		
		//计算右前轮角度
		akm_angle_r = 0;
	}	
	
    //根据右前轮角度，计算舵机转向角度
	akm_angle_s = 	-(0.0041*akm_angle_r*akm_angle_r + 1.2053*akm_angle_r)*180/PI;
	
	//根据舵机转向角度，计算舵机PWM控制量
	servo_angle = (akm_angle_s*10 + ax_akm_offset);  
	
	//设置舵机角度，S5,S6任意一个都可以
	AX_SERVO_S5_SetAngle(servo_angle);
	AX_SERVO_S6_SetAngle(servo_angle);	
	
	//利用PID算法计算电机PWM值
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);   
		
	//设置电机PWM值
	AX_MOTOR_A_SetSpeed(R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed(R_Wheel_B.PWM); 	
	
	//运动学正解析，由机器人轮子速度计算机器人速度
	R_Vel.RT_IX = ((R_Wheel_A.RT + R_Wheel_B.RT)/2)*1000;
	R_Vel.RT_IY = 0;
	R_Vel.RT_IW = ((-R_Wheel_A.RT + R_Wheel_B.RT)/AKM_WHEEL_BASE)*1000;	
}
#endif


#if (ROBOT_TYPE == ROBOT_TWD1)
/**
  * @简  述  机器人运动学处理-两轮差速
  * @参  数  无
  * @返回值  无
  */
void AX_ROBOT_Kinematics(void)
{
	
	//通过编码器获取车轮实时转速m/s
	R_Wheel_A.RT = (float) ((int16_t)AX_ENCODER_A_GetCounter()*TWD1_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float)-((int16_t)AX_ENCODER_B_GetCounter()*TWD1_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);			
	
	//调试输出轮子转速
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	//运动学正解析，由机器人轮子速度计算机器人速度
	R_Vel.RT_IX = ((R_Wheel_A.RT + R_Wheel_B.RT)/2)*1000;
	R_Vel.RT_IY = 0;
	R_Vel.RT_IW = ((-R_Wheel_A.RT + R_Wheel_B.RT)/TWD1_WHEEL_BASE)*1000;		
	
	//机器人目标速度限制
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	if( R_Vel.TG_IY > R_VY_LIMIT)     R_Vel.TG_IY = R_VY_LIMIT;
	if( R_Vel.TG_IY < (-R_VY_LIMIT))  R_Vel.TG_IY = (-R_VY_LIMIT);
	if( R_Vel.TG_IW > R_VW_LIMIT)     R_Vel.TG_IW = R_VW_LIMIT;
	if( R_Vel.TG_IW < (-R_VW_LIMIT))  R_Vel.TG_IW = (-R_VW_LIMIT);
	
	//目标速度转化为浮点类型
	R_Vel.TG_FX = R_Vel.TG_IX/1000.0;
	R_Vel.TG_FY = 0;
	R_Vel.TG_FW = R_Vel.TG_IW/1000.0;
	
	//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
	R_Wheel_A.TG = R_Vel.TG_FX - R_Vel.TG_FW*(TWD1_WHEEL_BASE/2);
	R_Wheel_B.TG = R_Vel.TG_FX + R_Vel.TG_FW*(TWD1_WHEEL_BASE/2);	
	

	//利用PID算法计算电机PWM值
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);  
 
	//设置电机PWM值
	AX_MOTOR_A_SetSpeed( R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed( R_Wheel_B.PWM); 	
 
	
	//printf("A%f B%f  \r\n ",MOTOR_A.Wheel_RT, MOTOR_B.Wheel_RT  );
	//printf("A%d B%d C%d  \r\n ",R_Vel.I_X, R_Vel.I_Y, R_Vel.I_W );	
	
}
#endif


/**
  * @简  述  机器人停止运动状态
  * @参  数  无
  * @返回值  无
  */
void AX_ROBOT_Stop(void)
{
	
	//设置电机PWM值
	AX_MOTOR_A_SetSpeed(0);
	AX_MOTOR_B_SetSpeed(0);    
	
}


    

