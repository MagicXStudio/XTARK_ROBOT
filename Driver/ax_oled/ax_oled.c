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
  * @内  容  OLED显示驱动程序
  *
  ******************************************************************************
  * @说  明
  *
  * 1.OLED为无字库128x64模组，驱动芯片为SSD1306，SPI驱动
  * 2.英文ASCII字符显示无须自己取模，已取模完成8X6和16x8规格，见ax_oled_ascii.h
  * 3.汉字和图片，需要用户自己取模，汉字规格16X16
  * 4.支持数字数值直接显示，可显示小数
  * 5.所有显示，字符、汉字、数字、图片均支持反色显示
  * 
  ******************************************************************************
  */
 
#include "ax_oled.h"
#include "ax_oled_ascii.h"

//OLED信号定义
#define OLED_DC_L()    GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define OLED_DC_H()    GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define OLED_RST_L()   GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define OLED_RST_H()   GPIO_SetBits(GPIOA,GPIO_Pin_6)

//函数定义
static uint8_t SPI1_Send_Byte(uint8_t dat);
static void OLED_WriteCmd(u8 data);
static void OLED_WriteData(u8 data);
static void OLED_Set_Pos(unsigned char x, unsigned char y); 

/**
  * @简  述  OLED初始化
  * @参  数  无	  
  * @返回值  无
  */
void AX_OLED_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//配置GPIO
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
  
	//配置SPI的SCK MOSI引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	//DC 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//配置 SPI 接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                  //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		              //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                      //选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                      //数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                      //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;    //定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                          //CRC值计算的多项式

	SPI_Init(SPI1, &SPI_InitStructure);

	//使能SPI外设
	SPI_Cmd(SPI1, ENABLE);
	
	//启动传输
	SPI1_Send_Byte(0xff);
	
    OLED_RST_H();
	AX_Delayms(100);
	OLED_RST_L();
	AX_Delayms(200);
	OLED_RST_H(); 
					  
	OLED_WriteCmd(0xAE);//--turn off oled panel
	OLED_WriteCmd(0x00);//---set low column address
	OLED_WriteCmd(0x10);//---set high column address
	OLED_WriteCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WriteCmd(0x81);//--set contrast control register
	OLED_WriteCmd(0xEF); // Set SEG Output Current Brightness
	OLED_WriteCmd(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WriteCmd(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WriteCmd(0xA6);//--set normal display
	OLED_WriteCmd(0xA8);//--set multiplex ratio(1 to 64)
	OLED_WriteCmd(0x3f);//--1/64 duty
	OLED_WriteCmd(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WriteCmd(0x00);//-not offset
	OLED_WriteCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WriteCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WriteCmd(0xD9);//--set pre-charge period
	OLED_WriteCmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WriteCmd(0xDA);//--set com pins hardware configuration
	OLED_WriteCmd(0x12);
	OLED_WriteCmd(0xDB);//--set vcomh
	OLED_WriteCmd(0x40);//Set VCOM Deselect Level
	OLED_WriteCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WriteCmd(0x02);//
	OLED_WriteCmd(0x8D);//--set Charge Pump enable/disable
	OLED_WriteCmd(0x14);//--set(0x10) disable
	OLED_WriteCmd(0xA4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WriteCmd(0xA6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WriteCmd(0xAF);//--turn on oled panel
	
	AX_OLED_ClearScreen();
}

/**
  * @简  述  OLED屏幕清除
  * @参  数  无
  * @返回值	 无
  */
void AX_OLED_ClearScreen(void)
{   	
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WriteCmd(0xb0+i);    //设置页地址（0~7）
		OLED_WriteCmd(0x00);      //设置显示位置—列低地址
		OLED_WriteCmd(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			OLED_WriteData(0); 
	} //更新显示
}


/**
  * @简  述  OLED指定位置显示一个ASCII字符（6X8）
  * @参  数  x：横坐标 0~123，超出范围不显示
  *          y：纵坐标 0~6，超出范围不显示
  *          ch：显示字符
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值	 
  */
void AX_OLED_DispChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t mode)
{   	
	uint8_t c=0,i=0;	

	c = ch - ' ';//得到偏移后的值
	
	//显示范围控制，超出部分不显示
	if((x<123) && (y<8))
	{
		if(mode != 0)
		{
			OLED_Set_Pos(x,y);
			for(i=0;i<6;i++)
				OLED_WriteData(~ASCII6X8[c*6+i]);		
		}
		else
		{
			OLED_Set_Pos(x,y);
			for(i=0;i<6;i++)
				OLED_WriteData(ASCII6X8[c*6+i]);
		}
	}
}


/**
  * @简  述  OLED指定位置显示ASCII字符串（8X16）,自动换行。
  * @参  数  x：横坐标 0~123，超出范围不显示
  *          y：纵坐标 0~7，超出范围不显示
  *          ch：字符串指针
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值	 
  */
void AX_OLED_DispStr(uint8_t x, uint8_t y, uint8_t *ch, uint8_t mode)
{   	
	uint8_t j=0;
	
	if((x<123) && (y<8))
	{
		while (ch[j]!='\0')
		{
			AX_OLED_DispChar(x, y, ch[j], mode);
				x+=6;
			
			if(x>122)
			{
				x=0;
				y+=1;  //当y>6时，超出部分不再显示
			}
			j++;
		}				
	}
}
/**
  * @简  述  OLED指定位置显示一个数字（8X16）
  * @参  数  x：横坐标 0~120，超出范围不显示
  *          y：纵坐标 0~6，超出范围不显示
  *          num：显示的数字
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值	 
  */
void AX_OLED_DispNum(uint8_t x, uint8_t y, uint8_t num, uint8_t mode)    //显示单个数字
{
	if(num < 10) 
	{	
		AX_OLED_DispChar(x, y, ('0'+num), mode);
	}
}
 
/**
  * @简  述  OLED指定位置显示数值（可显示负数）
  * @参  数  x：横坐标 0~122，超出范围不显示
  *          y：纵坐标 0~7，超出范围不显示
  *          value：显示的数值，32位整型数据，最大显示10位（inte + deci < 10）
  *          inte：整数位数，范围（>0, inte + deci < 10）
  *          deci：小数位数, 范围（inte + deci < 10）（取0时，为整数）
  *          mode：显示模式，0-正常显示，1-反白显示
  * @说  明  输入value为整型，如果是小数，可扩大相应倍数实现。例如123.45,先乘100，显示时整数位取3，小数位取2。
  *          如果显示数据可能为负数时，整数位数需要比最大位数多一位。例如-122，整数位取4。
  * @返回值	 
  */
void AX_OLED_DispValue(uint8_t x, uint8_t y, int32_t value, uint8_t inte, uint8_t deci, uint8_t mode)
{
	
	u8 dis_number[10];
	u8 i;
	u32 temp;
	
	if((inte>0) && ((inte+deci)<10))
	{
		if(value < 0)
			temp = (0-value);
		else
			temp = value;
		
		dis_number[1] = temp%10; 
		dis_number[2] = (temp%100)/10;
		dis_number[3] = (temp%1000)/100;
		dis_number[4] = (temp%10000)/1000;
		dis_number[5] = (temp%100000)/10000;
		dis_number[6] = (temp%1000000)/100000;
		dis_number[7] = (temp%10000000)/1000000;
		dis_number[8] = (temp%100000000)/10000000;
		dis_number[9] = (temp%1000000000)/100000000;		
		  
		if(value<0)
		{
		   AX_OLED_DispChar(x, y, '-', 0);
		   
		   for(i=0; i<(inte-1); i++ )
		   {
			   AX_OLED_DispNum(x+6+(i*6), y, dis_number[inte+deci-i-1],mode);
		   }
		   if(deci != 0 )
		   {
			   AX_OLED_DispChar(x+(inte*6), y, '.', mode);
			   
			   for(i=0; i<deci;i++ )
			   {
				   AX_OLED_DispNum(x+(inte*6)+6+(i*6), y, dis_number[deci-i],mode);
			   }
		   } 
		}
		else
		{	   
		   for(i=0; i<inte; i++ )
		   {
			   AX_OLED_DispNum(x+(i*6), y,dis_number[inte+deci-i],mode);
		   }
		   if(deci != 0 )
		   {
			   AX_OLED_DispChar(x+(inte*6), y, '.', mode);
			   
			   for(i=0; i<deci;i++ )
			   {
				   AX_OLED_DispNum(x+(inte*6)+6+(i*6), y, dis_number[deci-i], mode);
			   }
		   } 	   
		}
	}
	else // 参数错误
	{
		//AX_OLED_DisplayChar(x, y, 'X', 0);
	}	
}

/**
  * @简  述  OLED指定位置显示一个ASCII字符（8X16）
  * @参  数  x：横坐标 0~120，超出范围不显示
  *          y：纵坐标 0~6，超出范围不显示
  *          ch：显示字符
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值	 
  */
void AX_OLED_Disp16Char(uint8_t x, uint8_t y, uint8_t ch, uint8_t mode)
{   	
	uint8_t c=0,i=0;	

	c = ch - ' ';//得到偏移后的值
	
	//显示范围控制，超出部分不显示
	if((x<121) && (y<7))
	{
		if(mode != 0)
		{
			OLED_Set_Pos(x,y);
			for(i=0;i<8;i++)
				OLED_WriteData(~ASCII8X16[c*16+i]);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
				OLED_WriteData(~ASCII8X16[c*16+i+8]);			
		}
		else
		{
			OLED_Set_Pos(x,y);
			for(i=0;i<8;i++)
				OLED_WriteData(ASCII8X16[c*16+i]);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
				OLED_WriteData(ASCII8X16[c*16+i+8]);			
		}
		
	}
}

/**
  * @简  述  OLED指定位置显示ASCII字符串（8X16）,自动换行。
  * @参  数  x：横坐标 0~120，超出范围不显示
  *          y：纵坐标 0~6，超出范围不显示
  *          ch：字符串指针
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值	 
  */
void AX_OLED_Disp16Str(uint8_t x, uint8_t y, uint8_t *ch, uint8_t mode)
{   	
	uint8_t j=0;
	
	if((x<121) && (y<7))
	{
		while (ch[j]!='\0')
		{
			AX_OLED_Disp16Char(x, y, ch[j], mode);
				x+=8;
			
			if(x>120)
			{
				x=0;
				y+=2;  //当y>6时，超出部分不再显示
			}
			j++;
		}				
	}
}


/**
  * @简  述  OLED指定位置显示一个数字（8X16）
  * @参  数  x：横坐标 0~120，超出范围不显示
  *          y：纵坐标 0~6，超出范围不显示
  *          num：显示的数字
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值	 
  */
void AX_OLED_Disp16Num(uint8_t x, uint8_t y, uint8_t num, uint8_t mode)    //显示单个数字
{
	if(num < 10) 
	{	
		AX_OLED_Disp16Char(x, y, ('0'+num), mode);
	}
}
 
/**
  * @简  述  OLED指定位置显示数值（可显示负数）
  * @参  数  x：横坐标 0~120，超出范围不显示
  *          y：纵坐标 0~6，超出范围不显示
  *          value：显示的数值，32位整型数据，最大显示10位（inte + deci < 10）
  *          inte：整数位数，范围（>0, inte + deci < 10）
  *          deci：小数位数, 范围（inte + deci < 10）（取0时，为整数）
  *          mode：显示模式，0-正常显示，1-反白显示
  * @说  明  输入value为整型，如果是小数，可扩大相应倍数实现。例如123.45,先乘100，显示时整数位取3，小数位取2。
  *          如果显示数据可能为负数时，整数位数需要比最大位数多一位。例如-122，整数位取4。
  * @返回值	 
  */
void AX_OLED_Disp16Value(uint8_t x, uint8_t y, int32_t value, uint8_t inte, uint8_t deci, uint8_t mode)
{
	
	uint8_t dis_number[10];
	uint8_t i;
	u32 temp;
	
	if((inte>0) && ((inte+deci)<10))
	{
		if(value < 0)
			temp = (0-value);
		else
			temp = value;
		
		dis_number[1] = temp%10; 
		dis_number[2] = (temp%100)/10;
		dis_number[3] = (temp%1000)/100;
		dis_number[4] = (temp%10000)/1000;
		dis_number[5] = (temp%100000)/10000;
		dis_number[6] = (temp%1000000)/100000;
		dis_number[7] = (temp%10000000)/1000000;
		dis_number[8] = (temp%100000000)/10000000;
		dis_number[9] = (temp%1000000000)/100000000;		
		  
		if(value<0)
		{
		   AX_OLED_Disp16Char(x, y, '-', 0);
		   
		   for(i=0; i<(inte-1); i++ )
		   {
			   AX_OLED_Disp16Num(x+8+(i*8), y, dis_number[inte+deci-i-1],0);
		   }
		   if(deci != 0 )
		   {
			   AX_OLED_Disp16Char(x+(inte*8), y, '.', 0);
			   
			   for(i=0; i<deci;i++ )
			   {
				   AX_OLED_Disp16Num(x+(inte*8)+8+(i*8), y, dis_number[deci-i],0);
			   }
		   } 
		}
		else
		{	   
		   for(i=0; i<inte; i++ )
		   {
			   AX_OLED_Disp16Num(x+(i*8), y,dis_number[inte+deci-i],0);
		   }
		   if(deci != 0 )
		   {
			   AX_OLED_Disp16Char(x+(inte*8), y, '.', 0);
			   
			   for(i=0; i<deci;i++ )
			   {
				   AX_OLED_Disp16Num(x+(inte*8)+8+(i*8), y, dis_number[deci-i], 0);
			   }
		   } 	   
		}
	}
	else // 参数错误
	{
		//AX_OLED_DisplayChar(x, y, 'X', 0);
	}	
	
}


/**
  * @简  述  OLED指定位置显示一个汉字（16X16）
  * @参  数  x：横坐标 0~112，超出范围不显示
  *          y：纵坐标 0~6，超出范围不显示
  *          pbuf：汉字编码数据指针
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值	 
  */
void AX_OLED_DispChinese(uint8_t x, uint8_t y, const uint8_t *pbuf, uint8_t mode)
{      			    
	uint8_t t;

	if((x<113) && (y<7))
	{	
		if(mode)
		{
			OLED_Set_Pos(x,y);
			for(t=0;t<16;t++)
			{
				OLED_WriteData(~*pbuf++);
			}
			OLED_Set_Pos(x,y+1);	
			for(t=0;t<16;t++)
			{
				OLED_WriteData(~*pbuf++);
			}			
		}
		else
		{
			OLED_Set_Pos(x,y);
			for(t=0;t<16;t++)
			{
				OLED_WriteData(*pbuf++);
			}
			OLED_Set_Pos(x,y+1);	
			for(t=0;t<16;t++)
			{
				OLED_WriteData(*pbuf++);
			}	
		}
	}		
}


/**
  * @简  述  OLED指定位置显示一个指定尺寸照片
  * @参  数  x：图片起始点横坐标，0~127
  *          y：图片起始点纵坐标，0~7
  *          xsize：X轴图片尺寸，1~128
  *          ysize：Y轴图片尺寸，1~8
  *          pbuf：照片数据指针
  *          mode：显示模式，0-正常显示，1-反白显示
  * @返回值
  */
void AX_OLED_DispPicture(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, const uint8_t *pbuf, uint8_t mode)   
{
	uint8_t tx,ty;
	
	//参数过滤
	if(x>127 || y>7)
		return;
	if((x+xsize)>128 || (y+ysize)>8)
		return;

	if(mode)
	{
		for(ty=y; ty<ysize; ty++)
		{
			OLED_Set_Pos(x,ty);
			for(tx=x; tx<xsize; tx++)
			{
				OLED_WriteData(~*pbuf++);			
			}
		}		
		
	}
	else
	{
		for(ty=y; ty<ysize; ty++)
		{
			OLED_Set_Pos(x,ty);
			for(tx=x; tx<xsize; tx++)
			{
				OLED_WriteData(*pbuf++);			
			}
		}		
	}
	
}


/* 底层操作函数-----------------------------------------------------*/
/**
  * @简  述  SPI3写入读取一个字节函数
  * @参  数  dat：要写入的字节
  * @返回值  读出字节
  */
static uint8_t SPI1_Send_Byte(uint8_t dat)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI peripheral */
  SPI_I2S_SendData(SPI1, dat);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void OLED_WriteCmd(u8 data)
{	  
	OLED_DC_L();
	                  
    SPI1_Send_Byte(data);      //发送片擦除命令  

	OLED_DC_H(); 
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
static void OLED_WriteData(u8 data)
{	               
    SPI1_Send_Byte(data);      //发送片擦除命令  	
}


static void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WriteCmd(0xb0+y);
	OLED_WriteCmd(((x&0xf0)>>4)|0x10);
	OLED_WriteCmd((x&0x0f)|0x01); 
} 
/******************* (C) 版权 2023 XTARK **************************************/
