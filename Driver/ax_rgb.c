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
  * @内  容  WS2812 RGB灯带控制 
  *
  ******************************************************************************
  * @说  明
  *
  * 
  ******************************************************************************
  */

#include "ax_rgb.h"


// 硬件spi模拟ws2812时序（用spi的3位数据模拟ws2812的一位数据）
// 要求SPI的通信频率为2.25M，传输一位数据的时间约为444纳秒（ns）
//  __
// |  |_|   0b110  high level
//  _   
// | |__|   0b100  low level


#define TIM_ONE           0xF8
#define TIM_ZERO          0x80

//RGB彩灯显示数组
uint8_t RGB_BYTE_Buffer[PIXEL_NUM*24+2] = {0};  //头部和尾部增加0，消除显示错误

/**
  * @简  述  RGB彩灯初始化
  * @参  数  无
  * @返回值  无
  */
void AX_RGB_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	
	//GPIO配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);	
	
	//SPI 配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);   

	//配置 SPI 接口
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                  //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		              //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                      //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                      //数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                      //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;    //定义波特率预分频的值:波特率预分频值为256  //40M/8=5M
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                          //CRC值计算的多项式
	
	SPI_Init(SPI2, &SPI_InitStructure);

	//使能SPI外设
	SPI_Cmd(SPI2, ENABLE);
	SPI_CalculateCRC(SPI2, DISABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

	//DMA配置
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel5);
	
	//DMA配置
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SPI2->DR);           // 外设地址： SPIx  DR
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RGB_BYTE_Buffer;           // 待发送数据的地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          // 传送方向，从内存到寄存器
    DMA_InitStructure.DMA_BufferSize = 0;                                       // 发送的数据长度，初始化可设置为0，发送时修改
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     // 内存地址自动增加1
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     // 外设数据宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             // 内存数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               // 发送模式，只发一次
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         // DMA传送优先级为高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                // 关闭内存到内存
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);	
}


/**
  * @简  述  RGB彩灯更新数据
  * @参  数  无
  * @返回值  无
  */
void RGB_Update(void)
{
	
	//更新传输的数据量
	DMA_SetCurrDataCounter(DMA1_Channel5, PIXEL_NUM*24+2 );     // load number of bytes to be transferred
	
	// 使能DMA通道，开始传输数据	
	DMA_Cmd(DMA1_Channel5, ENABLE);                             // enable DMA channel 3

	// 等待传输完成
	while (!DMA_GetFlagStatus(DMA1_FLAG_TC5));  
    
	// 关闭DMA通道
    DMA_Cmd(DMA1_Channel5, DISABLE); 
	
	// 清除DMA通道状态
    DMA_ClearFlag(DMA1_FLAG_TC5);    
	
}



/**
  * @简  述  WS2812B设置每个灯的颜色
  * @参  数  uint32_t rgb：灯的颜色
  * @返回值  无
  */
void AX_RGB_SetFullColor( uint8_t r, uint8_t g, uint8_t b)
{
	
	uint16_t i,j;

	//给首尾加上0防止开始时发送数据错误导致时序错误
	RGB_BYTE_Buffer[0]=0;                               
	RGB_BYTE_Buffer[PIXEL_NUM*24 + 1]=0;	

	for(j=0;j<8;j++)
	{
		RGB_BYTE_Buffer[ j + 1] = ((g<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;
		RGB_BYTE_Buffer[ j + 1 + 8] = ((r<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;
		RGB_BYTE_Buffer[ j + 1 + 16] = ((b<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;			
	}

	for(i=1; i<PIXEL_NUM; i++ )
	{
		for(j=1;j<25;j++)
		{
			RGB_BYTE_Buffer[(24*i)+j] = RGB_BYTE_Buffer[j];  
		}		
	} 

	//RGB彩灯更新数据
	RGB_Update();
}


/**
  * @简  述  WS2812B设置每个灯的颜色
  * @参  数  uint8_t pixel[PIXEL_NUM][3] rgb像素 动态数组
  * @返回值  无
  */
void AX_RGB_SetPixelColor(uint8_t pixel[PIXEL_NUM][3])
{
	
	uint8_t i,j;
	
	//给首尾加上0防止开始时发送数据错误导致时序错误
    RGB_BYTE_Buffer[0]=0;                               
    RGB_BYTE_Buffer[PIXEL_NUM*24 + 1]=0;	
	
	for(i=0; i<PIXEL_NUM; i++ )
	{	
		for(j=0;j<8;j++)
		{
			RGB_BYTE_Buffer[(i*24) + j + 1]      = (( pixel[i][1]<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;
			RGB_BYTE_Buffer[(i*24) + j + 1 + 8]  = ((pixel[i][0]<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;
			RGB_BYTE_Buffer[(i*24) + j + 1 + 16] = (( pixel[i][2]<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;	
		}	
	}
	
	//RGB彩灯更新数据
    RGB_Update();
}

/**
  * @简  述  WS2812B设置每个灯的颜色
  * @参  数  uint8_t pixel[PIXEL_NUM][3] rgb像素 静态数组
  * @返回值  无
  */
void AX_RGB_SetPixelColor1(const uint8_t pixel[PIXEL_NUM][3])
{
	
	uint8_t i,j;
	
	//给首尾加上0防止开始时发送数据错误导致时序错误
    RGB_BYTE_Buffer[0]=0;                               
    RGB_BYTE_Buffer[PIXEL_NUM*24 + 1]=0;	
	
	for(i=0; i<PIXEL_NUM; i++ )
	{	
		for(j=0;j<8;j++)
		{
			RGB_BYTE_Buffer[(i*24) + j + 1]      = (( pixel[i][1]<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;
			RGB_BYTE_Buffer[(i*24) + j + 1 + 8]  = ((pixel[i][0]<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;
			RGB_BYTE_Buffer[(i*24) + j + 1 + 16] = (( pixel[i][2]<<j) & 0x0080) ? TIM_ONE:TIM_ZERO;	
		}	
	}

	//RGB彩灯更新数据
    RGB_Update();
	
}


/******************* (C) 版权 2023 XTARK **************************************/
