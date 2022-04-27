#ifndef __RELAY_H_
#define	__RELAY_H_

//定义IO口
sbit relay1 = P2^2;  //水循环继电器
sbit relay2 = P2^1;	 //加热继电器
sbit relay3 = P2^0;	 //充氧继电器


//水循环继电器
void add_water()
{
	relay1=0;
	delayms(100); //控制时间
	relay1=1;
}
//加热继电器
void add_temp()
{
	relay2=0;
	delayms(3000); //控制时间
	relay2=1;
}
//充氧继电器
void add_ox2()
{
	relay3=0;
	delayms(3000); //控制时间
	relay3=1;
}
#endif