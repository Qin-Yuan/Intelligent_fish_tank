#ifndef __MOTOR_H_
#define __MOTOR_H_

#include <reg52.h>
	
//定义ULN2003A的输入口
/*
sbit	IN0	= P1^7;
sbit	IN1	= P3^2;
sbit	IN2	= P2^4;
sbit	IN3	= P2^3;
*/
sbit	IN0	= P2^3;
sbit	IN1	= P2^4;
sbit	IN2	= P3^2;
sbit	IN3	= P1^7;

//正转一圈，time控制速度
void run_z(uint time)
{
	uchar i;
	for(i=0;i<1;i++)
	{
		
		
		IN0=0;IN1=1;IN2=0;IN3=0;delayms( time ); //0010
		IN0=0;IN1=1;IN2=1;IN3=0;delayms( time ); //0110
		IN0=0;IN1=0;IN2=1;IN3=0;delayms( time ); //0100
		IN0=0;IN1=0;IN2=1;IN3=1;delayms( time ); //1100
		IN0=0;IN1=0;IN2=0;IN3=1;delayms( time ); //1000
		IN0=1;IN1=0;IN2=0;IN3=1;delayms( time ); //1001
		IN0=1;IN1=0;IN2=0;IN3=0;delayms( time ); //0001
		IN0=1;IN1=1;IN2=0;IN3=0;delayms( time ); //0011
		
	};
}

//反转一圈，time控制速度
void run_f(uint time)
{
	uchar i;
	for(i=0;i<1;i++)
	{
		IN0=1;IN1=0;IN2=0;IN3=0;delayms( time ); //0001
		IN0=1;IN1=0;IN2=0;IN3=1;delayms( time ); //1001
		IN0=0;IN1=0;IN2=0;IN3=1;delayms( time ); //1000
		IN0=0;IN1=0;IN2=1;IN3=1;delayms( time ); //1100
		IN0=0;IN1=0;IN2=1;IN3=0;delayms( time ); //0100
		IN0=0;IN1=1;IN2=1;IN3=0;delayms( time ); //0110
		IN0=0;IN1=1;IN2=0;IN3=0;delayms( time ); //0010	
		IN0=1;IN1=1;IN2=0;IN3=0;delayms( time ); //0011
	};
}

#endif