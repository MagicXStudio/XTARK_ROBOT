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
  * 塔克微信： 微信公众号：塔克创新（获取最新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @日  期  2022-1-26
  * @内  容  机器人控制主函数
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "robot.h"
#include "speed.h"

//电机数据结构体
MOTOR_Data MOTOR_A,MOTOR_B;

//机器人速度数据
ROBOT_Velocity  RobotV_RT,RobotV_TG;

//机器人转向数据
ROBOT_Steering RobotStr;

//机器人IMU数据
ROBOT_IMU  IMU;

//电池电压
uint16_t ax_bat_vol;  

//舵机角度
int16_t ax_servo_offset = 0;

//IMU数据
int16_t ax_imu_acc_data[3];  
int16_t ax_imu_gyro_data[3]; 
int16_t ax_imu_gyro_offset[3]; 

//电机PID控制参数
int16_t ax_motor_kp=800;      
int16_t ax_motor_kd=800;

//IMU校准标志位
int8_t ax_imu_calibrate_flag = 0;

//函数定义
void ROBOT_IMUHandle(void);  //IMU数据处理
void ROBOT_Kinematics(void);    //运动学处理
void ROBOT_SendDataToPi(void);    //发送数据


/**
  * @简  述  机器人管理任务
  * @参  数  无
  * @返回值  无
  */
void Robot_Task(void* parameter)
{	
	
	//用于保存上次时间。调用后系统自动更新
	static portTickType PreviousWakeTime;

	//设置延时时间20ms，将时间转为节拍数 
	const portTickType TimeIncrement = pdMS_TO_TICKS(20);

	//获取当前系统时间 
	PreviousWakeTime = xTaskGetTickCount();

	while(1)
	{
		
		//调用绝对延时函数20ms,执行频率50HZ
		vTaskDelayUntil(&PreviousWakeTime, TimeIncrement );
		
		//获取PMU6050加速度数据
        ROBOT_IMUHandle();
		
		//机器人运动学处理
		ROBOT_Kinematics();	
		
		//数据发送
		ROBOT_SendDataToPi();
		
		//指示系统运行
		AX_LED_Green_Toggle();
	}
}

/**
  * @简  述  机器人IMU数据处理
  * @参  数  无
  * @返回值  无
  */
void ROBOT_IMUHandle(void)
{
		
	//获取PMU6050加速度数据
	AX_MPU6050_GetAccData(ax_imu_acc_data);
	
	//IMU坐标向机器人ROS坐标变换
	IMU.ACC_X = -ax_imu_acc_data[1];  //ROS坐标X轴对应IMU的Y轴反向
	IMU.ACC_Y = ax_imu_acc_data[0];  //ROS坐标Y轴对应IMU的X轴
	IMU.ACC_Z = ax_imu_acc_data[2];  //ROS坐标Z轴对应IMU的Z轴
	
	//获取PMU6050陀螺仪数据
	AX_MPU6050_GetGyroData(ax_imu_gyro_data);
	
	//陀螺仪加入零票校准数据
	ax_imu_gyro_data[0] += ax_imu_gyro_offset[0];
	ax_imu_gyro_data[1] += ax_imu_gyro_offset[1];
	ax_imu_gyro_data[2] += ax_imu_gyro_offset[2];
	
	//IMU坐标向机器人ROS坐标变换
	IMU.GYRO_X = -ax_imu_gyro_data[1];  //ROS坐标X轴对应IMU的Y轴反向
	IMU.GYRO_Y = ax_imu_gyro_data[0];  //ROS坐标Y轴对应IMU的X轴
	IMU.GYRO_Z = ax_imu_gyro_data[2];  //ROS坐标Z轴对应IMU的Z轴
}

/**
  * @简  述  机器人运动学处理
  * @参  数  无
  * @返回值  无
  */
void ROBOT_Kinematics(void)
{
	//舵机角度
	int16_t servo_angle;
	
	//通过编码器获取车轮实时转速m/s
	MOTOR_A.Wheel_RT = ((int16_t)AX_ENCODER_AB_GetCounter()*WHEEL_SCALE);
	AX_ENCODER_AB_SetCounter(0);
	MOTOR_B.Wheel_RT = -((int16_t)AX_ENCODER_CD_GetCounter()*WHEEL_SCALE);
	AX_ENCODER_CD_SetCounter(0);
	
	//机器人目标速度限制
	if( RobotV_TG.I_X > R_VX_LIMIT )    RobotV_TG.I_X = R_VX_LIMIT;
	if( RobotV_TG.I_X < (-R_VX_LIMIT))  RobotV_TG.I_X = (-R_VX_LIMIT);
	if( RobotV_TG.I_Y > R_VY_LIMIT)     RobotV_TG.I_Y = R_VY_LIMIT;
	if( RobotV_TG.I_Y < (-R_VY_LIMIT))  RobotV_TG.I_Y = (-R_VY_LIMIT);
	if( RobotV_TG.I_W > R_VW_LIMIT)     RobotV_TG.I_W = R_VW_LIMIT;
	if( RobotV_TG.I_W < (-R_VW_LIMIT))  RobotV_TG.I_W = (-R_VW_LIMIT);
	
	//目标速度转化为浮点类型
	RobotV_TG.F_X = RobotV_TG.I_X/1000.0;
	RobotV_TG.F_Y = RobotV_TG.I_Y/1000.0;
	RobotV_TG.F_W = RobotV_TG.I_W/1000.0;
	
	//判断机器人转向速度是否为0
	if(RobotV_TG.I_W != 0)
	{
		//判断机器人前进速度是否为0
		if( RobotV_TG.I_X != 0)
		{
			//计算转弯半径
			RobotStr.Radius =  RobotV_TG.F_X/RobotV_TG.F_W;
			
			//阿克曼机器人需要设置最小转弯半径
	        //如果目标速度要求的转弯半径小于最小转弯半径，
	        //会导致机器人运动摩擦力大大提高，严重影响控制效果

			//转弯半径小于最小转弯
			if(RobotStr.Radius>0 && RobotStr.Radius<R_TURN_R_MINI)
			{
				RobotStr.Radius = R_TURN_R_MINI; 
				
			}
				
			else if(RobotStr.Radius<0 && RobotStr.Radius>(-R_TURN_R_MINI))
			{
				RobotStr.Radius = -R_TURN_R_MINI;
			}
			
			//计算机器人前轮转向角度,单位弧度
			RobotStr.Angle = atan(R_ACLE_BASE/(RobotStr.Radius));				
				
			//运动学逆解析，由机器人目标速度计算电机轮子速度（m/s）
			MOTOR_A.Wheel_TG = RobotV_TG.F_X*(RobotStr.Radius-0.5*R_WHEEL_BASE)/RobotStr.Radius;
			MOTOR_B.Wheel_TG = RobotV_TG.F_X*(RobotStr.Radius+0.5*R_WHEEL_BASE)/RobotStr.Radius;				
		}
		else
		{
			MOTOR_A.Wheel_TG = 0;
			MOTOR_B.Wheel_TG = 0;
			RobotStr.Radius = 0;
			RobotStr.Angle = 0; 
		}
	}
	else
	{
		MOTOR_A.Wheel_TG = RobotV_TG.F_X;
		MOTOR_B.Wheel_TG = RobotV_TG.F_X;
		RobotStr.Radius = 0;
		RobotStr.Angle = 0;
	}
	
	//根据前轮角度计算右前轮角度
	if(RobotStr.Angle !=0 )
	{
		RobotStr.RAngle = (atan(R_ACLE_BASE/((R_ACLE_BASE/tan(RobotStr.Angle*0.01745))+0.5*R_WHEEL_BASE)))*(180.0/PI);
	}
	else
	{
		RobotStr.RAngle = 0;
	}
	
    //根据右前轮角度，计算舵机转向角度
	RobotStr.SAngle = 	-(0.0041*RobotStr.RAngle*RobotStr.RAngle + 1.2053*RobotStr.RAngle)*180/PI;
	
	//根据舵机转向角度，计算舵机PWM控制量
	servo_angle = (RobotStr.SAngle*10 + 900 + ax_servo_offset);  
	
	//利用PID算法计算电机PWM值
	MOTOR_A.Motor_Pwm = Motor_SpeedCtlA(MOTOR_A.Wheel_TG, MOTOR_A.Wheel_RT);   
	MOTOR_B.Motor_Pwm = Motor_SpeedCtlB(MOTOR_B.Wheel_TG, MOTOR_B.Wheel_RT);   
		
	//设置电机PWM值
	AX_MOTOR_A_SetSpeed(-MOTOR_A.Motor_Pwm);
	AX_MOTOR_B_SetSpeed(-MOTOR_B.Motor_Pwm);  
	
	//设置舵机角度
	AX_SERVO_S1_SetAngle(servo_angle);
	AX_SERVO_S2_SetAngle(servo_angle);
	AX_SERVO_S3_SetAngle(servo_angle);
	AX_SERVO_S4_SetAngle(servo_angle);
	
	//运动学正解析，由机器人轮子速度计算机器人速度
	RobotV_RT.I_X = ((MOTOR_A.Wheel_RT + MOTOR_B.Wheel_RT)/2)*1000;
	RobotV_RT.I_Y = 0;
	RobotV_RT.I_W = ((-MOTOR_A.Wheel_RT + MOTOR_B.Wheel_RT)/R_WHEEL_BASE)*1000;	
}

/**
  * @简  述  机器人发送数据到树莓派
  * @参  数  无
  * @返回值  无
  */
void ROBOT_SendDataToPi(void)
{
    //串口发送数据
	static uint8_t comdata[20]; 	

	//加速度 = (ax_acc/32768) * 2G  
	comdata[0] = (u8)( IMU.ACC_X >> 8 );  
	comdata[1] = (u8)( IMU.ACC_X );
	comdata[2] = (u8)( IMU.ACC_Y >> 8 );
	comdata[3] = (u8)( IMU.ACC_Y );
	comdata[4] = (u8)( IMU.ACC_Z >> 8 );
	comdata[5] = (u8)( IMU.ACC_Z );
	
	//陀螺仪角速度 = (ax_gyro/32768) * 500
	comdata[6] = (u8)( IMU.GYRO_X >> 8 );
	comdata[7] = (u8)( IMU.GYRO_X );
	comdata[8] = (u8)( IMU.GYRO_Y >> 8 );
	comdata[9] = (u8)( IMU.GYRO_Y );
	comdata[10] = (u8)( IMU.GYRO_Z	>> 8 );
	comdata[11] = (u8)( IMU.GYRO_Z );
	
	//机器人速度值 单位为m/s，放大1000倍
	comdata[12] = (u8)( RobotV_RT.I_X >> 8 );
	comdata[13] = (u8)( RobotV_RT.I_X );
	comdata[14] = (u8)( RobotV_RT.I_Y >> 8 );
	comdata[15] = (u8)( RobotV_RT.I_Y );
	comdata[16] = (u8)( RobotV_RT.I_W >> 8 );
	comdata[17] = (u8)( RobotV_RT.I_W );
	
	//电池电压
	comdata[18] = (u8)( ax_bat_vol >> 8 );
	comdata[19] = (u8)( ax_bat_vol );
		
	//发送串口数据
	AX_UART_PI_SendPacket(comdata, 20, ID_CPR2ROS_DATA);
}

/**
  * @简  述  电量管理任务
  * @参  数  无
  * @返回值  无
  */
void Bat_Task(void* parameter)
{	
	//计数变量
	static uint16_t ax_bat_vol_cnt = 0; 
	
	while (1)
	{	
		//采集电池电压
	    ax_bat_vol = AX_VIN_GetVol_X100();	
		
		//电量低于40%
		if(ax_bat_vol < VBAT_40P)  
		{
			//红灯开始闪烁警示
			AX_LED_Red_Toggle();
			
			//电量低于20%
			if(ax_bat_vol < VBAT_20P)
			{
				//红灯常亮
				AX_LED_Red_On();
				
				//电量低于10%，关闭系统进入保护状态
				if(ax_bat_vol < VBAT_10P) //990
				{
					//低压时间计数
					ax_bat_vol_cnt++;
					
					//超过10次，进入关闭状态
					if(ax_bat_vol_cnt > 10 )
					{
						//关闭绿灯，红灯常亮
						AX_LED_Green_Off();
						AX_LED_Red_On();
						
						//机器人任务挂起
						vTaskSuspend(Robot_Task_Handle);
						
						//电机速度设置为0
						AX_MOTOR_A_SetSpeed(0);
						AX_MOTOR_B_SetSpeed(0);  
						
						//蜂鸣器鸣叫报警
						while(1)
						{	
							AX_BEEP_On();
							vTaskDelay(30);
							AX_BEEP_Off();
							vTaskDelay(2000);						
						}								
					}
				}
				else
				{
					ax_bat_vol_cnt = 0;
				}				
			}
		}
		else
		{
			//红灯关闭
			AX_LED_Red_Off();
		}
		
        //循环周期200ms
		vTaskDelay(200); 
	}			
}


/**
  * @简  述  按键处理任务
  * @参  数  无
  * @返回值  无
  */
void Key_Task(void* parameter)
{	
	uint8_t i;
	
	while (1)
	{		
		//按键扫描
		if(AX_KEY_Scan() == 0)
		{
			//软件延时
			vTaskDelay(50);  
			
			//确定按键按下
			if(AX_KEY_Scan() == 0)
			{					
				//等待按键抬起
				while(AX_KEY_Scan() == 0)
				{
					vTaskDelay(50);	
				}
			}
		}
		
		//循环周期
		vTaskDelay(50);    
	}			
}

/**
  * @简  述  按键处理任务
  * @参  数  无
  * @返回值  无
  */
void Imu_Task(void* parameter)
{	
	uint8_t i;
	
	//陀螺仪校准变量
	static int16_t gyro_data[3]; 
	
	while (1)
	{		
		//循环周期
		vTaskDelay(500); 
		
		//检测IMU校准标志位
		if(ax_imu_calibrate_flag > 0)
		{
 
			//蜂鸣器提示
			AX_BEEP_On();
			vTaskDelay(200);
			AX_BEEP_Off();
			vTaskDelay(1000);
			
			ax_imu_gyro_offset[0] = 0;
			ax_imu_gyro_offset[1] = 0;
			ax_imu_gyro_offset[2] = 0;
			
			//机器人任务挂起
			vTaskSuspend(Robot_Task_Handle);
			
			//陀螺仪校准
			for(i=0; i<10; i++) 
			{
				//延时函数
				vTaskDelay(20);
				
				//获取PMU6050陀螺仪数据
				AX_MPU6050_GetGyroData(gyro_data);
				
				//计算偏差和
				ax_imu_gyro_offset[0] += gyro_data[0];
				ax_imu_gyro_offset[1] += gyro_data[1];
				ax_imu_gyro_offset[2] += gyro_data[2]; 		
			}
			
			//计算平均偏差值
			ax_imu_gyro_offset[0] = -ax_imu_gyro_offset[0]/10;
			ax_imu_gyro_offset[1] = -ax_imu_gyro_offset[1]/10;
			ax_imu_gyro_offset[2] = -ax_imu_gyro_offset[2]/10;
			
			//蜂鸣器提示
			AX_BEEP_On();
			vTaskDelay(50);
			AX_BEEP_Off();
			
			//机器人任务恢复
			vTaskResume(Robot_Task_Handle);
			
			//复位IMU校准标志位			
			ax_imu_calibrate_flag = 0;

		}   
	}			
}


/******************* (C) 版权 2022 XTARK **************************************/

