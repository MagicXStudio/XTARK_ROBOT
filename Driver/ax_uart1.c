/**			                                                        
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @内  容  调试串口通信
  *
  ******************************************************************************
  * @说  明
  *
  * 1.USB串口通信，printf函数已定向到该串口，可以输出调试信息
  * 2.可以使用具有X-Protocol协议进行数据发送。
  * 3.开启UART的串口数据接收功能，使用中断方式，X-Protocol协议通信
  * 4.可通过AX_UART1_GetRxData()函数判断是否有数据接收
  *
  * X-Protocol协议介绍（变帧长）
  * 帧定义：AA 55 | 0B  | 01  | 03 E8 FC 18 00 0A | 14
  *        帧头   帧长   帧码  数据                校验和
  * 帧  头：双帧头，抗干扰强
  * 帧  长：根据数据长度设定
  * 帧  码：用户根据功能设定，标识帧的唯一性
  * 数  据：高位在前，长度可变，内容自由组合8位，16位，32位数据
  * 校验和：前面数据累加和的低8位
  * 帧示例：( AA 55 0B 01 03 E8 FC 18 00 0A 14 ) 内容：1000，-1000,10,
  * 
  ******************************************************************************
  */

#include "ax_uart1.h"
#include <stdio.h>

#include "ax_robot.h"

static uint8_t uart1_rx_con=0;       //接收计数器
static uint8_t uart1_rx_checksum;    //帧头部分校验和
static uint8_t uart1_rx_buf[40];     //接收缓冲，数据内容小于等于32Byte
static uint8_t uart1_tx_buf[40];     //发送缓冲

/**
  * @简  述  UART 串口初始化
  * @参  数  baud： 波特率设置
  * @返回值	 无
  */
void AX_UART1_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//**调试串口USART配置******
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //打开串口GPIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //打开串口外设的时钟
	
	//将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置USART参数
	USART_InitStructure.USART_BaudRate = baud; //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	//配置USART为中断源
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢断优先级	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能中断
	NVIC_Init(&NVIC_InitStructure);//初始化配置NVIC
	
	//使能串口接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//使能 USART， 配置完毕
	USART_Cmd(USART1, ENABLE);
}

/**
  * @简  述  DBUART 串口中断服务函数
  * @参  数  无 
  * @返回值  无
  */
void USART1_IRQHandler(void)
{
	uint8_t Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(USART1);	
		
		if(uart1_rx_con < 3)    //==接收帧头 + 长度
		{
			if(uart1_rx_con == 0)  //接收帧头1 0xAA
			{
				if(Res == 0xAA)
				{
					uart1_rx_buf[0] = Res;
					uart1_rx_con = 1;					
				}
				else
				{
					
				}
			}else if(uart1_rx_con == 1) //接收帧头2 0x55
			{
				if(Res == 0x55)
				{
					uart1_rx_buf[1] = Res;
					uart1_rx_con = 2;
				}
				else
				{
					uart1_rx_con = 0;						
				}				
			}
			else  //接收数据长度
			{
				uart1_rx_buf[2] = Res;
				uart1_rx_con = 3;
				uart1_rx_checksum = (0xAA+0x55) + Res;	//计算校验和
			}
		}
		else    //==接收数据
		{
			if(uart1_rx_con < (uart1_rx_buf[2]-1) )
			{
				uart1_rx_buf[uart1_rx_con] = Res;
				uart1_rx_con++;
				uart1_rx_checksum = uart1_rx_checksum + Res;					
			}
			else    //判断最后1位
			{
				//接收完成，恢复初始状态
				uart1_rx_con = 0;	
				
				//数据校验
				if( Res == uart1_rx_checksum )  //校验正确
				{	

					//设置为ROS控制模式
					ax_control_mode = CTL_ROS;
					
					//速度控制帧
					if(uart1_rx_buf[3] == ID_URX_VEL)
					{
						R_Vel.TG_IX = (int16_t)((uart1_rx_buf[4]<<8) | uart1_rx_buf[5]);
						R_Vel.TG_IY = (int16_t)((uart1_rx_buf[6]<<8) | uart1_rx_buf[7]);
						R_Vel.TG_IW = (int16_t)((uart1_rx_buf[8]<<8) | uart1_rx_buf[9]);
						
						//如果是阿克曼机器人，根据矢量速度计算前轮转向角
						#if (ROBOT_TYPE == ROBOT_AKM)
							ax_akm_angle = AX_AKM_WToAngle(R_Vel.TG_IX, R_Vel.TG_IW);
						#endif
					}
					else
					{
						//IMU陀螺仪校准
						if(uart1_rx_buf[3] == ID_URX_IMU)
						{
							ax_imu_calibrate_flag = uart1_rx_buf[4];
						}	

						//RGB灯效控制帧
						if(uart1_rx_buf[3] == ID_URX_LG)
						{
							R_Light.M  = uart1_rx_buf[4];
							R_Light.S  = uart1_rx_buf[5];
							R_Light.T  = uart1_rx_buf[6];
							R_Light.R  = uart1_rx_buf[7];
							R_Light.G  = uart1_rx_buf[8];
							R_Light.B  = uart1_rx_buf[9];
						}

						//灯效EEPROM保存
						if(uart1_rx_buf[3] == ID_URX_LS)
						{
							//执行灯光效果保存动作
							ax_light_save_flag = 1;
						}	

						//蜂鸣器鸣叫控制
						if(uart1_rx_buf[3] == ID_URX_BP)
						{
							//设置为1，蜂鸣器鸣叫
							if(uart1_rx_buf[4] != 0)
							{
								AX_BEEP_On();
							}
							else
							{
								AX_BEEP_Off();
							}
						}							

						//机器人底盘型号信息，用于判断是否一致
						if(uart1_rx_buf[3] == ID_URX_RTY)
						{
							//判断与当前底盘是否一致，
							if(uart1_rx_buf[4] == ROBOT_TYPE  )
							{
								//一致，则蜂鸣器短鸣叫提示
								ax_beep_ring = BEEP_SHORT;
							}
							else
							{
								//不一致，则蜂鸣器长鸣叫报警
								ax_beep_ring = BEEP_LONG;								
							}
						}	
					}	
				}
			}
		}
		
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	} 
}


/**
  * @简  述  UART 发送数据（X-Protocol协议）
  * @参  数  *pbuf：发送数据指针
  *          len：发送数据长度个数，≤27 (32-5)
  *          num：帧号，帧编码
  * @返回值	 无
  */
void AX_UART1_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num)
{
	uint8_t i,cnt;	
  uint8_t tx_checksum = 0;//发送校验和
	
	if(len <= 32)
	{
		/******获取数据******/
		uart1_tx_buf[0] = 0xAA;    //帧头
		uart1_tx_buf[1] = 0x55;    //
		uart1_tx_buf[2] = len+5;  //根据输出长度计算帧长度
		uart1_tx_buf[3] = num;    //帧编码
		
		for(i=0; i<len; i++)
		{
			uart1_tx_buf[4+i] = *(pbuf+i);
		}
		
		/******计算校验和******/	
		cnt = 4+len;
		for(i=0; i<cnt; i++)
		{
			tx_checksum = tx_checksum + uart1_tx_buf[i];
		}
		uart1_tx_buf[i] = tx_checksum;
		
		
		/******发送数据******/	
		cnt = 5+len;
		
		//查询传输方式
		for(i=0; i<cnt; i++)
		{
			USART_SendData(USART1, uart1_tx_buf[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
		}
	}
}

/**************************串口打印相关函数重定义********************************/
/**
  * @简  述  重定义putc函数（USART1）	
  */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
}

/**
  * @简  述  重定义getc函数（USART1）	
  */
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{}

	return (int)USART_ReceiveData(USART1);
}

    
