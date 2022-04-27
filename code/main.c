#include <reg52.h>
#include <lcd.h>
#include <motor.h>
#include <temp.h>
#include <relay.h>
#include <key_scan.h>
#include <ds1302.h>
#include <i2c.h>

//局部变量
uint key1=1;  //键值记录
uint key2=0;  //记录上次按键
uint key3=0;  //按键设置参数
uint key4=0;  //按键判断增加还是减少
uint key5=0;  //无作用
int temp=250;     //记录采集的温度
uchar F_h=23;  //投食：h
uchar F_m=40;	 // m
uchar F_s=20;  // s
uchar O_h=23;  //充氧：h
uchar O_m=40;  // m
uchar O_s=35;  // s
uchar W_m=0; //换水周期分钟  <60 min
uchar W_s=5; //换水周期秒   <60 s
char h,m,s,h1,m1,s1,m2,s2; //记录读取EEPROM里的数据 小时：分：秒   h:投食设置时间   h1:充氧设置时间  m2：水循环周期

//全局变量
extern uchar temp_L=22;  //温度下限
extern uchar temp_H=26;  //温度上限

/*
24C02C外部储存模块
##### 温度 ####
0x00：温度上限    ox01：温度下限

##### 投食 ####
0x02：时  0x03：分  0x04：秒

##### 充氧 ####
0x05：时  0x06：分  0x07：秒

##### 换水 ####
0x08：分  0x09: 秒
*/


//函数声明
void LcdDisplay(int temp); 	 //lcd显示温度
void Timer0Init();   //定时器中断0初始化
void Timer1Init();   //定时器中断1初始化
void key_lcd();  //lcd_key操作函数
void datashow(); //LCD显示时钟时间
void i2c_init(); //EEPROM系统设置初始化
void key_set(); //参数设置


//主函数
void main()
{
	delayms(10);
	Ds18b20Init(); //温度传感器初始化，
	Ds1302Init();	   //时钟初始化，第一次初始化后就可以注释该条语句，这样下次重启就不会再次初始化了
	add_water(); //继电器
	InitLcd1602(); //初始化液晶
	//LcdStar(); //显示温度界面
	//delayms(3000);
	Timer0Init(); //定时器中断0初始化
	Timer1Init(); //定时器中断1初始化
	i2c_init();  //只需要执行一次
	while(1);
}

/*******************************************************************************
* 函 数 名         : datashow(int x,int y)
* 函数功能		     : LCD显示时间
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void datashow()	 
{
  LcdSetCursor(4,0); 
	LcdWriteDat('0'+TIME[2]/16);				//时
	LcdWriteDat('0'+(TIME[2]&0x0f));				 
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[1]/16);				//分
	LcdWriteDat('0'+(TIME[1]&0x0f));	
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[0]/16);				//秒
	LcdWriteDat('0'+(TIME[0]&0x0f));
	/*
	LcdWriteCmd(0x80+0X40);
	LcdWriteDat('2');
	LcdWriteDat('0');
	LcdWriteDat('0'+TIME[6]/16);			//年
	LcdWriteDat('0'+(TIME[6]&0x0f));
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[4]/16);			//月
	LcdWriteDat('0'+(TIME[4]&0x0f));
	LcdWriteDat(':');
	LcdWriteDat('0'+TIME[3]/16);			//日
	LcdWriteDat('0'+(TIME[3]&0x0f));	 
	*/
}
/*******************************************************************************
* 函 数 名         : work()
* 函数功能		     : 投食、充氧时间检测
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void work()
{
	//投食
	if((TIME[2]/16)==(h/10)&&(TIME[2]&0x0f)==(h%10)&&(TIME[1]/16)==(m/10)&&(TIME[1]&0x0f)==(m%10)&&(TIME[0]/16)==(s/10)&&(TIME[0]&0x0f)==(s%10))
	{
		int n=200;
		while(n--){
		run_z(1);}
		delayms(1000);
		n=200;
		while(n--){
		run_f(1);}
	}
	//充氧
	else if((TIME[2]/16)==(h1/10)&&(TIME[2]&0x0f)==(h1%10)&&(TIME[1]/16)==(m1/10)&&(TIME[1]&0x0f)==(m1%10)&&(TIME[0]/16)==(s1/10)&&(TIME[0]&0x0f)==(s1%10))
	{
		add_ox2();
	}
}
/*******************************************************************************
* 函数名         : i2c()
* 函数功能		   : 初始化写入数据到EEPROM
* 输入           : v
* 输出         	 : 无
*******************************************************************************/
void i2c_init()
{
	At24c02Write(0x00,temp_H);   //温度最低
	delayms(5);
	At24c02Write(0x01,temp_L);   //温度最高
	delayms(5);
	At24c02Write(0x02,F_h);   //自动投食：h
	delayms(5);
	At24c02Write(0x03,F_m);   //自动投食：m
	delayms(5);
	At24c02Write(0x04,F_s);   //自动投食：s
	delayms(5);
	At24c02Write(0x05,O_h);   //自动充氧：h
	delayms(5);
	At24c02Write(0x06,O_m);   //自动充氧：m
	delayms(5);
	At24c02Write(0x07,O_s);   //自动充氧：s
	delayms(5);
	At24c02Write(0x08,W_m);   //自动换水：m
	delayms(5);
	At24c02Write(0x09,W_s);   //自动换水：s
	delayms(5);
}

/*******************************************************************************
* 函数名         : LcdDisplay()
* 函数功能		   : LCD显示读取到的温度
* 输入           : v
* 输出         	 : 无
*******************************************************************************/
void LcdDisplay(int temp) 	 //lcd显示
{
    uchar datas[] = {0, 0, 0, 0, 0}; //记录温度000.00,只显示00.00
		float tp; 
	  //获取投食设置的时间
		delayms(5);
		h = At24c02Read(0x02);
		delayms(5);
		m = At24c02Read(0x03);
		delayms(5);
		s = At24c02Read(0x04);
		//获取充氧设置的时间
		delayms(5);
		h1 = At24c02Read(0x05);
		delayms(5);
		m1 = At24c02Read(0x06);
		delayms(5);
		s1 = At24c02Read(0x07); 
		delayms(5);
		//换水周期
		m2 = At24c02Read(0x08);
		delayms(5);
		s2 = At24c02Read(0x09); 
		delayms(5);
	  if(temp< 0)				//当温度值为负数
			{
				//LcdSetCursor(5,1);  		//显示负
				//LcdWriteDat('-');
				//因为读取的温度是实际温度的补码，所以减1，再取反求出原码
				temp=temp-1;
				temp=~temp;
				tp=temp;
				temp=tp*0.0625*100+0.5;	
				//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
				//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5
				//算加上0.5，还是在小数点后面。
			}
 	  else
			{			
				//LcdSetCursor(5,1); 		//显示正
				//LcdWriteDat('+');
				tp=temp;//因为数据处理有小数点所以将温度赋给一个浮点型变量
				//如果温度是正的那么，那么正数的原码就是补码它本身
				temp=tp*0.0625*100+0.5;	
				//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
				//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
				//算加上0.5，还是在小数点后面。
			}
		//datas[0] = temp / 10000;			 //白
		datas[1] = temp % 10000 / 1000;  //十
		datas[2] = temp % 1000 / 100;		 //个
		datas[3] = temp % 100 / 10;			 // .0
		//datas[4] = temp % 10;					 // .00
		
		//判断是否需要加热
		if((temp/100)<temp_L)
		{
			relay2=0;
		}
		else  //((temp/100)>temp_H)
		{
			relay2=1;
		}
		//判断按键切换界面
		if(key1==1)
		{
			if(key2!=1)
			{
				LcdWriteCmd(0x01); //清屏
				key2 = 1 ;
			}
			//实时显示温度
		  //LcdSetCursor(6,1); //百位 
	    //LcdWriteDat('0'+datas[0]);
			LcdShowStr(1,0,"Temp:");
			LcdSetCursor(7,0); //十位
			LcdWriteDat('0'+datas[1]);
			LcdSetCursor(8,0); //个位 
			LcdWriteDat('0'+datas[2]);
			LcdSetCursor(9,0); 		//显示 ‘.’
			LcdWriteDat('.');
			LcdSetCursor(10,0); //显示小数点 
			LcdWriteDat('0'+datas[3]);	
			//LcdSetCursor(11,1); //显示小数点 
			//LcdWriteDat('0'+datas[4]);
			//显示温度范围设置
			LcdShowStr(1,1,"L: ");
			LcdWriteDat('0'+temp_L/10);
			LcdWriteDat('0'+temp_L%10);
			LcdShowStr(7,1,"H: ");
			LcdWriteDat('0'+temp_H/10);
			LcdWriteDat('0'+temp_H%10);
		}
		
		//LCD显示自动投食时间
		else if(key1==2)
		{
			if(key2!=2)
			{
				LcdWriteCmd(0x01); //清屏
				key2 = 2;
				LcdShowStr(0,1,"F_time: ");
				LcdWriteDat('0'+h/10);
				LcdWriteDat('0'+h%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+m/10);
				LcdWriteDat('0'+m%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+s/10);
				LcdWriteDat('0'+s%10);
			}
			datashow();
		}
		
		//LCD显示自动充氧时间
		else if(key1==3)
		{
			if(key2!=3)
			{
				LcdWriteCmd(0x01); //清屏
				key2 = 3;
				LcdShowStr(0,1,"O_time: ");
				LcdWriteDat('0'+h1/10);
				LcdWriteDat('0'+h1%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+m1/10);
				LcdWriteDat('0'+m1%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+s1/10);
				LcdWriteDat('0'+s1%10);
			}
			datashow();
		}
		//LCD显示自动水循环时间
		else if(key1==4)
		{
			if(key2!=4)
			{
				LcdWriteCmd(0x01); //清屏
				key2 = 4;
				LcdShowStr(4,1,"W_T: ");
				LcdWriteDat('0'+m2/10);
				LcdWriteDat('0'+m2%10);
				LcdWriteDat(':');
				LcdWriteDat('0'+s2/10);
				LcdWriteDat('0'+s2%10);
			}
			datashow();
		}
}

/*
###############################
#######  定时器中断0 ##########
########## 初始化 #############
*/
void Timer0Init()
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0 = (65535-20000)/256;	 //20MS定时
  TL0 = (65535-20000)%256;
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;//打开定时器
}
//#####  定时器0的任务 #######
void timer0() interrupt 1
{
	static uint i,j;
	TR0 = 0;
	TH0 = (65536-20000)/256;	 //20MS定时
  TL0 = (65536-20000)%256;
	key_scan(&key1,&key3); //按键扫描，并把按键值传递给key1
	key_set();
	work();
	i++;
	j++;
	//充水
	if(j==30*(m2*60+s2))
	{
		relay1=0;
	}
	else if(j==30*(m2*60+s2+1))
	{
		relay1=1;
		j=0;
	}
	/*
	if(i==40)
	{	
		temp = temp+50;
		//LcdDisplay(temp) ;   //LCD温度值刷新
		i=0;
	}
	if(temp==500)
	{temp=250;}
	*/
	TR0=1;
}
/*
###############################
#######  定时器中断1 ##########
########## 初始化 #############
*/
void Timer1Init()
{
	TMOD|=0X10;//选择为定时器1模式，工作方式1，仅用TR1打开启动。
	TH0 = (65536-20000)/256;	 //20MS定时
  TL0 = (65536-20000)%256;
	ET1=1;//打开定时器1中断允许
	EA=1;//打开总中断
	TR1=1;//打开定时器
}
//###### 定时器1的任务 #####
void timer1() interrupt 3
{
	TR1=0;
	TH0 = (65535-20000)/256;	 //20MS定时
  TL0 = (65535-20000)%256;
	Ds1302ReadTime(); //时钟TIME
	LcdDisplay(Ds18b20ReadTemp()); //更新LCD显示
	//Ds18b20ReadTemp(); //更新温度值
	TR1=1;
}

/*
#################################
#######  key设置参数 #######
*///11111
void key_set()
{
	//投食时间设定
	if(key3==2)
	{
		LcdWriteCmd(0x01);
		LcdShowStr(5,0,"ft_set");
		while(1)
		{	
			key_scan(&key4,&key5);
			//显示当前的值
			LcdSetCursor(4,1);
			LcdWriteDat('0'+h/10);
			LcdWriteDat('0'+h%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+m/10);
			LcdWriteDat('0'+m%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+s/10);
			LcdWriteDat('0'+s%10);
			if(key4==3)
			{
				key4=0;
				m--;
				if(m<0)
				{
					m=59;
					h--;
					if(h<0)
					{	
						h=23;
					}
				}
				At24c02Write(0x02,h);   //自动投食：h
				delayms(5);
				At24c02Write(0x03,m);   //自动充氧：m 
				delayms(5);
			}
			else if(key4==4)
			{
				key4=0;
				m++;
				if(m>59)
				{
					m=0;
					h++;
					if(h>23)
					{	
						h=0;
					}
				}
				At24c02Write(0x02,h);   //自动投食：h
				delayms(5);
				At24c02Write(0x03,m);   //自动投食：m 
				delayms(5);
			}
			else if(key4==1)
			{
				key2=key3=key4=0;
				break;
			}
		}
	}
	//冲氧时间设定
	else if(key3==3)
	{
		LcdWriteCmd(0x01);
		LcdShowStr(5,0,"Ot_set");
		while(1)
		{	
			key_scan(&key4,&key5);
			//显示当前的值
			LcdSetCursor(4,1);
			LcdWriteDat('0'+h1/10);
			LcdWriteDat('0'+h1%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+m1/10);
			LcdWriteDat('0'+m1%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+s1/10);
			LcdWriteDat('0'+s1%10);
			if(key4==2)
			{
				key4=0;
				m1--;
				if(m1<0)
				{
					m1=59;
					h1--;
					if(h1<0)
					{	
						h1=23;
					}
				}
				At24c02Write(0x05,h1);   //自动充氧：h
				delayms(5);
				At24c02Write(0x06,m1);   //自动充氧：m 
				delayms(5);
			}
			else if(key4==4)
			{
				key4=0;
				m1++;
				if(m1>59)
				{
					m1=0;
					h1++;
					if(h1>23)
					{	
						h1=0;
					}
				}
				At24c02Write(0x05,h1);   //自动投充：h
				delayms(5);
				At24c02Write(0x06,m1);   //自动充氧：m 
				delayms(5);
				
			}
			else if(key4==1)
			{
				key2=key3=key4=0;
				break;
			}
		}
	}
	//水循环周期设定
	else if(key3==4)
	{
		LcdWriteCmd(0x01);
		LcdShowStr(5,0,"Wt_set");
		while(1)
		{	
			key_scan(&key4,&key5);
			//显示当前的值
			LcdSetCursor(6,1);
			LcdWriteDat('0'+m2/10);
			LcdWriteDat('0'+m2%10);
			LcdWriteDat(':');
			LcdWriteDat('0'+s2/10);
			LcdWriteDat('0'+s2%10);
			if(key4==2)
			{
			  key4=0;
				s2=s2-5;
				if(s2<0)
				{
					s2=59;
					m2--;
					if(m2<0)
					{	
						m2=59;
					}
				}
				At24c02Write(0x08,m2);   //自动冲氧：m
				delayms(5);
				At24c02Write(0x09,s2);   //自动充氧：s 
				delayms(5);
			}
			else if(key4==3)
			{
			  key4=0;
				s2=s2+5;
				if(s2>59)
				{
					s2=0;
					m2++;
					if(m2>59)
					{	
						m2=0;
					}
				}
				At24c02Write(0x08,m2);   //自动冲氧：m
				delayms(5);
				At24c02Write(0x09,s2);   //自动充氧：s 
				delayms(5);
			}
			else if(key4==1)
			{
				key2=key3=key4=0;
				break;
			}
		}
	}
}