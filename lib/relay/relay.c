#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int
	
//定义IO口
sbit relay1 = P2^2;  //水循环继电器
sbit relay2 = P2^1;	 //加热继电器
sbit relay3 = P2^0;	 //充氧继电器

//延时函数
void delay( uint ms )
{
	uchar i;
	while ( ms-- )
	{
		for (i=0;i<120;i++);
	}
}

//水循环继电器
void add_water()
{
	relay1=0;
	delay(3000); //控制时间
	relay1=1;
}
//加热继电器
void add_temp()
{
	relay2=0;
	delay(3000); //控制时间
	relay2=1;
}
//充氧继电器
void add_ox2()
{
	relay3=0;
	delay(3000); //控制时间
	relay3=1;
}

void main()
{
	while(1)
	{
	relay1=0;
	relay2=0;
	relay3=0;
	}
}