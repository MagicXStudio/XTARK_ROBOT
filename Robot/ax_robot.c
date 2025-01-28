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
  * @内  容  机器人控制主函数
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_robot.h"
#include "ax_light.h"
#include "ax_kinematics.h"
#include "ax_control.h"

//机器人速度数据
ROBOT_Velocity  R_Vel;

//机器人轮子数据
ROBOT_Wheel  R_Wheel_A,R_Wheel_B;

//机器人IMU数据
ROBOT_Imu  R_Imu;

//机器人RGB数据
ROBOT_Light  R_Light;

//机器人速度数据
ROBOT_Velocity  R_Vel;

//机器人电池电压数据
uint16_t R_Bat_Vol;  

//IMU数据
int16_t ax_imu_acc_data[3];  
int16_t ax_imu_gyro_data[3]; 
int16_t ax_imu_gyro_offset[3]; 

//电机PID控制参数
int16_t ax_motor_kp=600;      
int16_t ax_motor_kd=800; 

//机器人运动使能开关,默认打开状态
uint8_t ax_robot_move_enable = 1;

//IMU校准标志位
int8_t ax_imu_calibrate_flag = 0;

//灯光开关失能，默认打开状态
uint8_t ax_light_enable = 1;

//灯光效果，效果保存
uint8_t ax_light_save_flag = 0;

//蜂鸣器鸣叫一声
uint8_t ax_beep_ring = 0;

//PS2手柄键值结构体
JOYSTICK_TypeDef my_joystick;  

//控制方式选择
uint8_t ax_control_mode = CTL_ROS;

//机器人掉电保存参数变量
uint16_t ax_param[10];

//参数区域保存
uint8_t ax_param_flag = 0;

//阿克曼机器人前轮转向角度
int16_t ax_akm_angle = 0;

//阿克曼机器人前轮零偏角度
int16_t ax_akm_offset = 0;

//函数定义
void ROBOT_IMUHandle(void);     //IMU数据处理
void ROBOT_SendDataToRos(void);  //发送数据
	
	
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
		
		//控制方式选择
		if(ax_control_mode != 0)
		{
			if      (ax_control_mode == CTL_PS2)    AX_CTL_Ps2();    //PS2手柄控制
			else if (ax_control_mode == CTL_APP)    AX_CTL_App();    //APP控制
			else if (ax_control_mode == CTL_RMS)    AX_CTL_RemoteSbus();   //SBUS航模遥控器控制
		}
		
		//判断机器人运动是否开启
		if(ax_robot_move_enable != 0)
		{
			//机器人运动学处理
			AX_ROBOT_Kinematics();				
		}
		else
		{
			//机器人静止不动
			AX_ROBOT_Stop();				
		}			
		
		//获取PMU6050加速度数据
        ROBOT_IMUHandle();
		
		//数据发送
		ROBOT_SendDataToRos();
			
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
	R_Imu.ACC_X =  ax_imu_acc_data[1];  //ROS坐标X轴对应IMU的Y轴
	R_Imu.ACC_Y = -ax_imu_acc_data[0];  //ROS坐标Y轴对应IMU的X轴反向
	R_Imu.ACC_Z =  ax_imu_acc_data[2];  //ROS坐标Z轴对应IMU的Z轴
	
	//获取PMU6050陀螺仪数据
	AX_MPU6050_GetGyroData(ax_imu_gyro_data);
	
	//陀螺仪加入零票校准数据
	ax_imu_gyro_data[0] += ax_imu_gyro_offset[0];
	ax_imu_gyro_data[1] += ax_imu_gyro_offset[1];
	ax_imu_gyro_data[2] += ax_imu_gyro_offset[2];
	
	//IMU坐标向机器人ROS坐标变换
	R_Imu.GYRO_X =  ax_imu_gyro_data[1];  //ROS坐标X轴对应IMU的Y轴
	R_Imu.GYRO_Y = -ax_imu_gyro_data[0];  //ROS坐标Y轴对应IMU的X轴反向
	R_Imu.GYRO_Z =  ax_imu_gyro_data[2];  //ROS坐标Z轴对应IMU的Z轴
	
	//观察数据
    //printf("@ %d  \r\n",R_Imu.ACC_X);
}

/**
  * @简  述  机器人发送数据到ROS
  * @参  数  无
  * @返回值  无
  */
void ROBOT_SendDataToRos(void)
{
    //串口发送数据
	static uint8_t comdata[20]; 	

	//加速度 = (ax_acc/32768) * 2G  
	comdata[0] = (u8)( R_Imu.ACC_X >> 8 );  
	comdata[1] = (u8)( R_Imu.ACC_X );
	comdata[2] = (u8)( R_Imu.ACC_Y >> 8 );
	comdata[3] = (u8)( R_Imu.ACC_Y );
	comdata[4] = (u8)( R_Imu.ACC_Z >> 8 );
	comdata[5] = (u8)( R_Imu.ACC_Z );
	
	//陀螺仪角速度 = (ax_gyro/32768) * 500
	comdata[6] = (u8)( R_Imu.GYRO_X >> 8 );
	comdata[7] = (u8)( R_Imu.GYRO_X );
	comdata[8] = (u8)( R_Imu.GYRO_Y >> 8 );
	comdata[9] = (u8)( R_Imu.GYRO_Y );
	comdata[10] = (u8)( R_Imu.GYRO_Z	>> 8 );
	comdata[11] = (u8)( R_Imu.GYRO_Z );
	
	//机器人速度值 单位为m/s，放大1000倍
	comdata[12] = (u8)( R_Vel.RT_IX >> 8 );
	comdata[13] = (u8)( R_Vel.RT_IX );
	comdata[14] = (u8)( R_Vel.RT_IY >> 8 );
	comdata[15] = (u8)( R_Vel.RT_IY );
	comdata[16] = (u8)( R_Vel.RT_IW >> 8 );
	comdata[17] = (u8)( R_Vel.RT_IW );
	
	//电池电压
	comdata[18] = (u8)( R_Bat_Vol >> 8 );
	comdata[19] = (u8)( R_Bat_Vol );

	//TTL串口发送数据
    AX_UART4_SendPacket(comdata, 20, ID_UTX_DATA);		
	
	//USB串口发送数据（与调试串口共用，需要调试时可注释掉该句）
    AX_UART1_SendPacket(comdata, 20, ID_UTX_DATA);	
}


/**
  * @简  述  琐事管理任务
  * @参  数  无
  * @返回值  无
  */
void Trivia_Task(void* parameter)
{	
	uint8_t i;	
	
	//计数变量
	static uint16_t ax_bat_vol_cnt = 0; 
	
	//陀螺仪校准变量
	static int16_t gyro_data[3]; 
	

	while (1)
	{	
		
		/*****电池管理***********************************/
		
		//采集电池电压
	    R_Bat_Vol = AX_VIN_GetVol_X100();
		
		//调试输出电池电压数据
        //printf("@ %d  \r\n",R_Bat_Vol);		
		
		//电量低于40%
		if(R_Bat_Vol < VBAT_40P)  
		{
			//红灯开始闪烁警示
			AX_LED_Red_Toggle();
			
			//电量低于20%
			if(R_Bat_Vol < VBAT_20P)
			{
				//红灯常亮
				AX_LED_Red_On();
				
				//电量低于10%，关闭系统进入保护状态
				if(R_Bat_Vol < VBAT_10P) //990
				{
					//低压时间计数
					ax_bat_vol_cnt++;
					
					//超过10次，进入关闭状态
					if(ax_bat_vol_cnt > 10 )
					{
						//关闭绿灯，红灯常亮
						AX_LED_Green_Off();
						AX_LED_Red_On();
						
						//任务挂起
						vTaskSuspend(Robot_Task_Handle);
						vTaskSuspend(Disp_Task_Handle);
						
						//电机速度设置为0
						AX_MOTOR_A_SetSpeed(0);
						AX_MOTOR_B_SetSpeed(0);  
						
						//清除OLED启动画面显示
						AX_OLED_ClearScreen();  //
						
						//蜂鸣器鸣叫报警
						while(1)
						{	
							//显示机器人停止信息
							AX_OLED_DispStr(0, 3, "     Low power      ", 0);	
							AX_OLED_DispStr(0, 5, "  Robot has stopped ", 0);	
							AX_BEEP_On();
							vTaskDelay(30);
							AX_BEEP_Off();
							
							vTaskDelay(1000);	
							AX_OLED_ClearScreen(); 
							vTaskDelay(1000);						
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
		
		/*****IMU校准***********************************/
		
		//检测IMU校准标志位
		if(ax_imu_calibrate_flag > 0)
		{
            //付零IMU校准变量
			ax_imu_gyro_offset[0] = 0;
			ax_imu_gyro_offset[1] = 0;
			ax_imu_gyro_offset[2] = 0;			
			
			//机器人任务挂起
			vTaskSuspend(Robot_Task_Handle);			
			
			//蜂鸣器提示
			AX_BEEP_On();
			vTaskDelay(50);
			AX_BEEP_Off();
			vTaskDelay(1000);
			
			//点亮红灯提示
			AX_LED_Red_On();
			
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
			
			//关闭红灯
			AX_LED_Red_Off();
			
			//机器人任务恢复
			vTaskResume(Robot_Task_Handle);
			
			//复位IMU校准标志位			
			ax_imu_calibrate_flag = 0;

		}   	
		
		
		/*****更新FLASH参数区域数据************************************/
		if(ax_param_flag > 0)
		{
			//机器人任务挂起
			vTaskSuspend(Robot_Task_Handle);
			
			//更新数据
			ax_param[1] = R_Light.M;
			ax_param[2] = R_Light.S;
			ax_param[3] = R_Light.T;
			ax_param[4] = R_Light.R;
			ax_param[5] = R_Light.G;
			ax_param[6] = R_Light.B;
			
			//阿克曼舵机零偏修正参数	
			ax_param[7] = (uint16_t)ax_akm_offset;
			
			//写入数据
			AX_FLASH_Write(0x00, ax_param, 10);
 
			//蜂鸣器提示
			AX_BEEP_On();
			vTaskDelay(200);
			AX_BEEP_Off();
			vTaskDelay(1000);	
			
			//机器人任务恢复
			vTaskResume(Robot_Task_Handle);
			
			//复位参数保存标志位			
			ax_param_flag = 0;
			
		}			
		
		/*****蜂鸣器鸣叫管理***********************************/
		if(ax_beep_ring != 0)
		{
			if(ax_beep_ring == BEEP_SHORT)
			{
				//短鸣叫一声
				AX_BEEP_On();
				vTaskDelay(200); 
				AX_BEEP_Off();
				
				//鸣叫一声标志复位
				ax_beep_ring = 0;
			}
			else //
			{
				//长鸣叫一声
				AX_BEEP_On();
				vTaskDelay(1000); 
				AX_BEEP_Off();
				
				//鸣叫一声标志复位
				ax_beep_ring = 0;				
			}
		}
		
		//LED系统心跳指示
		AX_LED_Green_Toggle();	
		
		
        //循环周期500ms
		vTaskDelay(500); 
	}			
}

/**
  * @简  述  按键处理任务
  * @参  数  无
  * @返回值  无
  */
void Key_Task(void* parameter)
{	
	uint8_t  i;
	int16_t  temp;
	
	while (1)
	{		
		//按键扫描
		if(AX_KEY_Scan() != 0)
		{
			//软件延时
			vTaskDelay(50);  
			
			//确定按键按下
			if(AX_KEY_Scan() != 0)
			{
				//等待按键抬起
				for(i=0; i<200; i++)
				{

					vTaskDelay(50);
					
					if(AX_KEY_Scan() == 0)
					{
						break;
					}
					
					//按键时长3S时，给出提示音
					if(i == 60)
					{
						AX_BEEP_On();
						vTaskDelay(200);
						AX_BEEP_Off();
					}
				}

				//短按检测,小于1S
				if(i < 20)
				{
					//灯光效果切换
					if(R_Light.M < LEFFECT6)
						R_Light.M++;
					else
						R_Light.M = LEFFECT1;
					
					//IMU陀螺仪校准
					//ax_imu_calibrate_flag = 1;
					
				}
				
				//中按检测,大于3S，小于10S
				if(i>60 && i<200)
				{
					//判断是否在AKM模式下
					#if (ROBOT_TYPE == ROBOT_AKM)
					
					//机器人运动解析停止，机器人静止
					ax_robot_move_enable = 0;
					
					//阿克曼校准	
					AX_OLED_DispStr(0, 4, "                     ", 0);							
					AX_OLED_DispStr(0, 4, " AKM offset:", 1);
					
					//灯带按键按下
					while(1)
					{
						
						//显示实时零偏修正角度
						AX_OLED_DispValue(78, 4, (ax_akm_offset), 4, 0, 0);
						
						//右轮B电机，速度量
						temp = ((int16_t)AX_ENCODER_B_GetCounter()*AKM_WHEEL_SCALE*10);
						AX_ENCODER_B_SetCounter(0);	
						
						//偏置角度做增量
						if(temp > 0)
						{
							if(ax_akm_offset<120)
							{
								ax_akm_offset += temp;
							}
							else
							{
								//蜂鸣器报警提示
								AX_BEEP_On();
								vTaskDelay(30);
								AX_BEEP_Off();
							}
						}
						
						if(temp < 0)
						{
							if(ax_akm_offset>-120)
							{
								ax_akm_offset += temp;
							}
							else
							{
								//蜂鸣器报警提示
								AX_BEEP_On();
								vTaskDelay(30);
								AX_BEEP_Off();
							}
						}
						
						//设置舵机角度
						AX_SERVO_S5_SetAngle(ax_akm_offset);
						AX_SERVO_S6_SetAngle(ax_akm_offset);	
						
						vTaskDelay(50);
						
						//按键按下，推出校准
						if(AX_KEY_Scan() != 0)
						{
							break;
						}
					}
					
					//执行参数区域保存动作
					ax_param_flag = 1;						
					
					//蜂鸣器鸣叫提示
					AX_BEEP_On();
					vTaskDelay(500);
					AX_BEEP_Off();	
					vTaskDelay(800);	
					
					//机器人运动解析启动
					ax_robot_move_enable = 0;
					
					//恢复OLED显示
					AX_OLED_DispStr(0, 4, "---------------------", 0);	
						
					#endif

				}				
				
					
				//长按检测，大于10S
				if(i == 200)
				{
					//任务挂起
					vTaskSuspend(Robot_Task_Handle);
					vTaskSuspend(Disp_Task_Handle);		

					//蜂鸣器提示
					AX_BEEP_On();
					vTaskDelay(3000);
					AX_BEEP_Off();
					
					//电机速度设置为0
					AX_MOTOR_A_SetSpeed(0);
					AX_MOTOR_B_SetSpeed(0);  
					
					//清除OLED启动画面显示
					AX_OLED_ClearScreen(); 
					
   				    //清除FLASH参数标记，恢复默认设置
					ax_param[0] = 0x0000; 
					
					//写入数据
					AX_FLASH_Write(0x00, ax_param, 10);					
					
					//蜂鸣器鸣叫报警
					while(1)
					{	
						//显示机器人停止信息
						AX_OLED_DispStr(0, 3, "  Robot has stopped  ", 0);	
						AX_OLED_DispStr(0, 5, "    Please Restart   ", 0);	
						
						AX_BEEP_On();
						vTaskDelay(10);
						AX_BEEP_Off();
							
						vTaskDelay(1500);	
						AX_OLED_ClearScreen(); 
						vTaskDelay(500);						
					}					
				}
			}
		}
		
		//循环周期
		vTaskDelay(50);    
	}			
}


/**
  * @简  述  灯光处理任务
  * @参  数  无
  * @返回值  无
  */
void Light_Task(void* parameter)
{	

	while (1)
	{	
        //延时30ms，30HZ显示频率		
		vTaskDelay(30); 
		
		//判断前灯是否打开
		if(ax_light_enable != 0)
		{
			//正常显示
			AX_LIGHT_Show();
		}
		else
		{
			//关闭状态
			AX_RGB_SetFullColor(0x00, 0x00, 0x00);
		}
	}			
}


/**
  * @简  述  按键处理任务
  * @参  数  无
  * @返回值  无
  */
void Disp_Task(void* parameter)
{	

	while (1)
	{	
		//延时	
		vTaskDelay(100); 
		
		//显示控制模式
		if(ax_control_mode)
		{
			if      (ax_control_mode == CTL_PS2)    AX_OLED_DispStr(90, 2, "PS2", 0);   //PS2手柄控制
			else if (ax_control_mode == CTL_APP)    AX_OLED_DispStr(90, 2, "APP", 0);   //APP控制
			else if (ax_control_mode == CTL_RMS)    AX_OLED_DispStr(90, 2, "RMS", 0);   //SBUS航模遥控器控制			
		}
		else
		{
			//ROS控制模式
			AX_OLED_DispStr(90, 2, "ROS", 0);
		}
		
		//显示电池电压，陀螺仪Z轴数据,舵机偏角,
		AX_OLED_DispValue(30, 3, (R_Bat_Vol*0.1), 2, 1, 0);
		AX_OLED_DispValue(90, 3, (R_Imu.GYRO_Z ), 6, 0, 0);	
		
		vTaskDelay(100); 
		//显示轮子实时速度
		AX_OLED_DispValue(30, 6, (R_Wheel_A.RT*100), 2, 2, 0);
		AX_OLED_DispValue(90, 6, (R_Wheel_B.RT*100), 2, 2, 0);	
	}			
}

/**
  * @简  述  PS2数据获取任务
  * @参  数  无
  * @返回值  无
  */
void Ps2_Task(void* parameter)
{	

	//用于保存上次时间。调用后系统自动更新
	static portTickType PreviousWakeTime1;

	//设置延时时间20ms，将时间转为节拍数 
	const portTickType TimeIncrement1 = pdMS_TO_TICKS(20);
	
	//获取当前系统时间 
	PreviousWakeTime1 = xTaskGetTickCount();
	
	while(1)
	{
		
		//调用绝对延时函数20ms,执行频率50HZ
		vTaskDelayUntil(&PreviousWakeTime1, TimeIncrement1 );
		
		//读取PS2手柄键值
		AX_PS2_ScanKey(&my_joystick);
		
		//不在PS2控制模式下
		if(ax_control_mode != CTL_PS2)
		{
			//判断是否开启PS2手柄控制
			//START按键被按下后，左边摇杆上推，进入PS2控制模式
			if((my_joystick.btn1 == PS2_BT1_START) && (my_joystick.LJoy_UD == 0x00))
			{
				//切换到PS2模式
				ax_control_mode = CTL_PS2;	

				//执行蜂鸣器鸣叫提示
				ax_beep_ring = BEEP_SHORT;
			}
		}
		
//		//打印手柄键值
//		printf("MODE:%2x BTN1:%2x BTN2:%2x RJOY_LR:%2x RJOY_UD:%2x LJOY_LR:%2x LJOY_UD:%2x\r\n",
//		my_joystick.mode, my_joystick.btn1, my_joystick.btn2, 
//		my_joystick.RJoy_LR, my_joystick.RJoy_UD, my_joystick.LJoy_LR, my_joystick.LJoy_UD);
	}
}

/******************* (C) 版权 2023 XTARK **************************************/

