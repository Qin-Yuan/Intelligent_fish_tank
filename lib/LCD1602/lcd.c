#include <reg52.h>

#define LCD1602_DB P0
sbit LCD1602_E=P2^7;
sbit LCD1602_RW=P2^5;
sbit LCD1602_RS=P2^6;

void InitLcd1602();
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

//液晶初始界面——温度
void LcdStar()
{
	unsigned char str[] = "benny";
	unsigned char tab[]="temp: "; 
	InitLcd1602();//初始化液晶
	LcdShowStr(5, 0, str);
	LcdShowStr(1, 1, tab);
	LcdShowStr(6, 1, "00.00");//默认初始化温度为00
	LcdShowStr(12, 1, "C");//添加C温度	
}

//等待液晶准备
void LcdWaitReady()
{
	unsigned char sta;
	LCD1602_DB = 0xFF;
	LCD1602_RS = 0;
	LCD1602_RW = 1;
	do {
		LCD1602_E = 1;
		sta = LCD1602_DB; //读取状态字
		LCD1602_E = 0;
	} while (sta & 0x80); //bit7等于1表示液晶正忙，重复检测直到等于0为止
}

//向LCD1602液晶写入一字节命令cmd
void LcdWriteCmd(unsigned char cmd)
{
	LcdWaitReady();
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_DB = cmd;
	LCD1602_E = 1;
	LCD1602_E = 0;
}
//向LCD1602液晶写入一字节数据dat
void LcdWriteDat(unsigned char dat)
{
	LcdWaitReady();
	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_DB = dat;
	LCD1602_E = 1;
	LCD1602_E = 0;
}
//设置显示RAM起始位置，即光标位置(x,y)
void LcdSetCursor(unsigned char x, unsigned char y)
{
	unsigned char addr;
	if (y == 0) //由输入的屏幕坐标计算RAM的地址
		addr = 0x00 + x; //第一行：0x00
	else
		addr = 0x40 + x; //第二行：0x40
	LcdWriteCmd(addr | 0x80); //设置RAM地址
}
//在液晶(x,y)坐标处显示字符串str
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str)
{
	LcdSetCursor(x, y); //设置起始坐标
	while (*str != '\0') //连续写入字符串数据，直到结束符
	{
		LcdWriteDat(*str++); //自加1
	}
}
//初始化1602液晶
void InitLcd1602()
{
	LcdWriteCmd(0x38); //16*2显示, 5*7点阵, 8位数据接口
	LcdWriteCmd(0x0C); //显示器开，光标关闭
	LcdWriteCmd(0x06); //文字不动，地址自动+1
	LcdWriteCmd(0x01); //清屏
}

//测试
void main()
{
	InitLcd1602(); //初始化液晶
	//LcdStar(); //显示温度界面
	LcdSetCursor(4,0);
	LcdWriteDat('0');
	LcdWriteDat('1');
	LcdWriteDat('2');
	while(1);
}