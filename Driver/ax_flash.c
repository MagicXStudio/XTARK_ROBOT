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
  * @内  容  FLASH读写
  *
  ******************************************************************************
  */

#include "ax_flash.h"
#include <stdio.h>


//STM32F103RCT6扇区总共有0~127个扇区，每个扇区大小为2K
//建议选择最后几个扇区，程序一般用不到的位置
#define TK_FLASH_DATA_SECTOR    126   
                  
//FLASH解算关键字
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)				  
				  
//STM32F103RCT6的FLASH规格参数                 
#define TK_FLASH_SIZE      256              //所选STM32的FLASH容量大小(单位为K)
#define TK_FLASH_WREN      1                //使能FLASH写入(0，不使能;1，使能)
#define TK_SECTOR_SIZE     2048             //大容量每个扇区定义为2K
#define TK_FLASH_BASE      0x08000000      //STM32 FLASH的起始地址
#define TK_FLASH_END       0x0803FFFF      //STM32F103RCT6 FLASH的结束地址

//FLASH读写缓存空间
uint16_t flash_buf[TK_SECTOR_SIZE / 2]; //2K字节

//内部函数定义
static void Flash_Unlock(void);
static void Flash_Lock(void);
static uint8_t Flash_GetStatus(void);
static uint8_t Flash_WaitDone(uint16_t time);
static uint8_t Flash_WriteHalfWord(uint32_t faddr, uint16_t dat);
static uint16_t Flash_ReadHalfWord(uint32_t faddr);
static void Flash_Write_NoCheck(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite);
static uint8_t Flash_ErasePage(uint32_t paddr);
static void Flash_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);
static void Flash_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite);


/**
  * @简  述  从指定地址开始读出指定长度的数据
  * @参  数  addr:起始地址，此地址必须为2的倍数，范围0~2048
             pbuff:数据指针
             num:半字(16位)数
  * @返回值	 无
  */
void AX_FLASH_Read(uint16_t addr, uint16_t *pbuff, uint8_t num)
{
	uint32_t flash_addr;	  
	
	//计算实际地址
	flash_addr = TK_FLASH_BASE + TK_FLASH_DATA_SECTOR * TK_SECTOR_SIZE + addr;
	
	//读取数据
	Flash_Read(flash_addr, pbuff, num);
}

/**
  * @简  述  从指定地址开始写入指定长度的数据
  * @参  数  addr:起始地址，此地址必须为2的倍数，范围0~2048
             pbuff:数据指针
             NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
  * @返回值	 无
  */
void AX_FLASH_Write(uint16_t addr, uint16_t *pbuff, uint8_t num)
{
	uint32_t flash_addr;

	
	
	//计算实际地址
	flash_addr = TK_FLASH_BASE + TK_FLASH_DATA_SECTOR * TK_SECTOR_SIZE + addr;
	
	//写入数据
	Flash_Write(flash_addr, pbuff, num);
}

/**
  * @简  述  擦除整个页
  * @参  数  无
  * @返回值	 无
  */
void AX_FLASH_Erase(void)
{
	//擦除选定的页
	Flash_ErasePage(TK_FLASH_DATA_SECTOR);
}


//内部函数-------------------------------------------------------------------------------------

/**
  * @简  述  解锁STM32的FLASH
  * @参  数  无
  * @返回值	 无
  */
static void Flash_Unlock(void)
{
	//写入解锁序列
	FLASH->KEYR = FLASH_KEY1; 
	FLASH->KEYR = FLASH_KEY2;
}


/**
  * @简  述  上锁STM32的FLASH
  * @参  数  无
  * @返回值	 无
  */
static void Flash_Lock(void)
{
	 //上锁
	FLASH->CR |= 1 << 7;
}

/**
  * @简  述  得到FLASH状态
  * @参  数  无
  * @返回值	 状态
  */
static uint8_t Flash_GetStatus(void)
{
	uint32_t res;
	res = FLASH->SR;
	
	//判断状态
	if (res & (1 << 0))
		return 1; //忙
	else if (res & (1 << 2))
		return 2; //编程错误
	else if (res & (1 << 4))
		return 3; //写保护错误
	return 0;	  //操作完成
}

/**
  * @简  述  等待操作完成解
  * @参  数  time:要延时的长短
  * @返回值	 状态
  */
static uint8_t Flash_WaitDone(uint16_t time)
{
	uint8_t res, i;
	
	//循环判断
	do
	{
		res = Flash_GetStatus();
		if (res != 1)
			break; //非忙,无需等待了,直接退出.
		for (i = 0; i < 10; i++); // 增加延迟
		time--;
	} while (time);
	
	
	//TIMEOUT
	if (time == 0)
		res = 0xff; 
	
	return res;
}

/**
  * @简  述  在FLASH指定地址写入半字
  * @参  数  dat:要写入的数据
  * @返回值	 写入的情况
  */
static uint8_t Flash_WriteHalfWord(uint32_t faddr, uint16_t dat)
{
	uint8_t res;
	
	res = Flash_WaitDone(0XFF);
	
	//OK
	if (res == 0) 
	{
		FLASH->CR |= 1 << 0;		   //编程使能
		*(vu16 *)faddr = dat;		   //写入数据
		res = Flash_WaitDone(0XFF); //等待操作完成
		if (res != 1)				   //操作成功
		{
			FLASH->CR &= ~(1 << 0); //清除PG位.
		}
	}
	return res;
}

/**
  * @简  述  读取指定地址的半字(16位数据)
  * @参  数  faddr:读地址
  * @返回值	 对应数据
  */
static uint16_t Flash_ReadHalfWord(uint32_t faddr)
{
	return *(vu16 *)faddr;
}

/**
  * @简  述  不检查的写入
  * @参  数  WriteAddr:起始地址
  *          pBuffer:数据指针
  *          NumToWrite:半字(16位)数
  * @返回值	 无
  */
static void Flash_Write_NoCheck(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
	uint16_t i;
	for (i = 0; i < NumToWrite; i++)
	{
		Flash_WriteHalfWord(WriteAddr, pBuffer[i]);
		WriteAddr += 2; //地址增加2.
	}
}

/**
  * @简  述  擦除页
  * @参  数  paddr:页地址
  * @返回值	 执行情况
  */
static uint8_t Flash_ErasePage(uint32_t paddr)
{
	uint8_t res = 0;
	
	//等待上次操作结束,>20ms
	res = Flash_WaitDone(0X5FFF); 
	
	
	if (res == 0)
	{
		FLASH->CR |= 1 << 1;			 //页擦除
		FLASH->AR = paddr;				 //设置页地址
		FLASH->CR |= 1 << 6;			 //开始擦除
		res = Flash_WaitDone(0X5FFF);    //等待操作结束,>20ms
		if (res != 1)					 //非忙
		{
			FLASH->CR &= ~(1 << 1); //清除页擦除标志.
		}
	}
	
	return res;	
}




/**
  * @简  述  从指定地址开始读出指定长度的数据
  * @参  数  ReadAddr:起始地址
             pBuffer:数据指针
             NumToWrite:半字(16位)数
  * @返回值	 无
  */
static void Flash_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead)
{
	uint16_t i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = Flash_ReadHalfWord(ReadAddr);   //读取2个字节.
		ReadAddr += 2;								 //偏移2个字节.
	}
}

/**
  * @简  述  从指定地址开始写入指定长度的数据
  * @参  数  WriteAddr:起始地址(此地址必须为2的倍数!!)
             pBuffer:数据指针
             NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
  * @返回值	 无
  */
static void Flash_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
	uint32_t secpos;	   //扇区地址
	uint16_t secoff;	   //扇区内偏移地址(16位字计算)
	uint16_t secremain;    //扇区内剩余地址(16位字计算)
	uint16_t i;
	uint32_t offaddr;      //去掉0X08000000后的地址
	
	 //非法地址
	if (WriteAddr < TK_FLASH_BASE || (WriteAddr >= (TK_FLASH_BASE + 1024 * TK_FLASH_SIZE)))
		return;								 
	
	 //解锁
	Flash_Unlock();						 
	
	//计算地址
	offaddr = WriteAddr - TK_FLASH_BASE;	  //实际偏移地址.
	secpos = offaddr / TK_SECTOR_SIZE;		  //扇区地址  0~127 for STM32F103RCT6
	secoff = (offaddr % TK_SECTOR_SIZE) / 2; //在扇区内的偏移(2个字节为基本单位.)
	secremain = TK_SECTOR_SIZE / 2 - secoff; //扇区剩余空间大小
	
	//不大于该扇区范围
	if (NumToWrite <= secremain)
		secremain = NumToWrite; 
	
	//写入操作
	while (1)
	{
		//读出整个扇区的内容
		Flash_Read(secpos * TK_SECTOR_SIZE + TK_FLASH_BASE, flash_buf, TK_SECTOR_SIZE / 2); 
		
		//校验数据
		for (i = 0; i < secremain; i++)															 
		{
			if (flash_buf[secoff + i] != 0XFFFF)
				break; //需要擦除
		}
		
		//需要擦除
		if (i < secremain) 
		{
			//擦除这个扇区
			Flash_ErasePage(secpos * TK_SECTOR_SIZE + TK_FLASH_BASE); 
			
			//复制数据
			for (i = 0; i < secremain; i++)								 
			{
				flash_buf[i + secoff] = pBuffer[i];
			}
			
			//写入整个扇区
			Flash_Write_NoCheck(secpos * TK_SECTOR_SIZE + TK_FLASH_BASE, flash_buf, TK_SECTOR_SIZE / 2); 
		}
		else
			Flash_Write_NoCheck(WriteAddr, pBuffer, secremain);   //写已经擦除了的,直接写入扇区剩余区间.
		
		//判断是否写入结束
		if (NumToWrite == secremain)
			break; //写入结束了
		else	   //写入未结束
		{
			secpos++;					//扇区地址增1
			secoff = 0;					//偏移位置为0
			pBuffer += secremain;		//指针偏移
			WriteAddr += secremain * 2; //写地址偏移(16位数据地址,需要*2)
			NumToWrite -= secremain;	//字节(16位)数递减
			if (NumToWrite > (TK_SECTOR_SIZE / 2))
				secremain = TK_SECTOR_SIZE / 2; //下一个扇区还是写不完
			else
				secremain = NumToWrite; //下一个扇区可以写完了
		}
	};
	
	//上锁
	Flash_Lock(); 
}

/******************* (C) 版权 2023 XTARK **************************************/
