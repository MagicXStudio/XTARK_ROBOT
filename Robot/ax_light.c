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
  * @内  容  机器人灯效控制
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_light.h"
#include "ax_robot.h"

//灯的像素动态数组
uint8_t light_pixel[8][3] =
{
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
};

//灯效模式预定义
void Light_Effect1(void);
void Light_Effect2(void);
void Light_Effect3(void);
void Light_Effect4(void);
void Light_Effect5(void);
void Light_Effect6(void);


/**
  * @简  述  灯效显示函数
  * @参  数  无
  * @返回值  无
  */
void AX_LIGHT_Show(void)
{

	//显示模式1
	if(R_Light.M == LEFFECT1)
	{
		  Light_Effect1();
	}
	
	//显示模式2
	else if(R_Light.M == LEFFECT2)
	{
		  Light_Effect2();
	}
	
	//显示模式3
	else if(R_Light.M == LEFFECT3) 
	{
		 Light_Effect3();
	}
	
	//显示模式4
	else if(R_Light.M == LEFFECT4) 
	{
		 Light_Effect4();
	}
	
	//显示模式5
	else if(R_Light.M == LEFFECT5) 
	{
		 Light_Effect5();
	}
	
	//显示模式6
	else if(R_Light.M == LEFFECT6) 
	{
		 Light_Effect6();
	}
	//超出范围，默认效果1
	else 
	{
		 Light_Effect1();
	}

}		
	

/**
  * @简  述  灯光效果，单色效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect1(void)
{
	
	AX_RGB_SetFullColor(R_Light.R, R_Light.G, R_Light.B);
	 
}


/**
  * @简  述  灯光效果，呼吸效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect2(void)
{
	 static uint8_t cnt;
	 uint8_t ar,ag,ab;
	
	 uint8_t light;
	
	 //颜色筛选
	 ar = ( R_Light.R<128 ) ? 0:1;
	 ag = ( R_Light.G<128 ) ? 0:1;
	 ab = ( R_Light.B<128 ) ? 0:1;
	
	 if(cnt<41)  //变亮
	 {
		 light = cnt*6;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else if (cnt<61)  //保持
	 {
		 light = 240;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else if (cnt<79)  //变暗
	 {
		 light =  (80-cnt)*8;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else if (cnt<91)  //保持
	 {
		 light =  12;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else
	 {
		 cnt = 2;
	 }
	 
	 cnt++;
}

 //自定义彩色
const uint8_t light3_pixel[8][3] =
{
	{0xFF,0x00,0x00},{0xFF,0xFF,0x00},{0x00,0xFF,0x00},{0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF},{0x00,0x00,0xFF},{0x00,0xFF,0xFF},{0xFF,0x00,0xFF},
};

/**
  * @简  述  灯光效果，彩色效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect3(void)
{

	AX_RGB_SetPixelColor1(light3_pixel);
	
}

//红灯
const uint8_t light4_pixel_red[8][3] =
{
	{0x00,0x00,0x00},{0xFF,0x00,0x00},{0xFF,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
};
//蓝灯
const uint8_t light4_pixel_blue[8][3] =
{
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0xFF},{0x00,0x00,0xFF},{0x00,0x00,0x00},
};
/**
  * @简  述  灯光效果，警灯效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect4(void)
{

	 static uint8_t cnt;


	 if(cnt<3)  //红灯
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_red);
	 }
	 else if (cnt<6)  //熄灭
	 {
		 AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else if (cnt<10)  //红灯
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_red);
	 }
	 else if (cnt<20)  //熄灭长时间
	 {
		AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else if(cnt<23)  //蓝灯
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_blue);
	 }
	 else if (cnt<26)  //熄灭
	 {
		 AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else if (cnt<30)  //蓝灯
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_blue);
	 }
	 else if (cnt<40)  //熄灭长时间
	 {
		AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else
	 {
		 cnt = 0;
	 }
	 
	 cnt++;

}


/**
  * @简  述  灯光效果，眼睛效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect5(void)
{
	uint8_t i;
	
	light_pixel[0][0] = R_Light.R;
	light_pixel[0][1] = R_Light.G;
	light_pixel[0][2] = R_Light.B;
	
	light_pixel[7][0] = R_Light.R;
	light_pixel[7][1] = R_Light.G;
	light_pixel[7][2] = R_Light.B;
	
	for(i=1;i<7;i++)
	{
		light_pixel[i][0] = 0;
		light_pixel[i][1] = 0;
		light_pixel[i][2] = 0;
	}
	
	AX_RGB_SetPixelColor(light_pixel);
}


/**
  * @简  述  灯光效果，闪电效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect6(void)
{
	static uint8_t i;
	
	//正方向
	if(i<8) 
	{
		//熄灭前一个灯
		if(i!=0)
		{
			light_pixel[i-1][0] = 0;
			light_pixel[i-1][1] = 0;
			light_pixel[i-1][2] = 0;	
		}

		//点亮当前灯
		light_pixel[i][0] = R_Light.R;
		light_pixel[i][1] = R_Light.G;
		light_pixel[i][2] = R_Light.B;	
		
		i++;
	}
	//反方向
	else if(i<16)
	{
		//熄灭前一个灯
		if(i!=8)
		{
			light_pixel[15-i+1][0] = 0;
			light_pixel[15-i+1][1] = 0;
			light_pixel[15-i+1][2] = 0;		
		}
		
		//点亮当前灯
		light_pixel[15-i][0] = R_Light.R;
		light_pixel[15-i][1] = R_Light.G;
		light_pixel[15-i][2] = R_Light.B;	
		
		i++;
	}
	else
	{
		i = 0;
	}
		
	//发送数据
	AX_RGB_SetPixelColor(light_pixel);
}


/******************* (C) 版权 2023 XTARK **************************************/

