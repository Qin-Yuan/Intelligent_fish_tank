#include<reg52.h>
#include<intrins.h>

//---¶¨Òåds1302Ê¹ÓÃµÄIO¿Ú---//
sbit DSIO=P1^2;
sbit RST=P1^3;
sbit SCLK=P1^1;

//---DS1302Ð´ÈëºÍ¶ÁÈ¡Ê±·ÖÃëµÄµØÖ·ÃüÁî---//
//---Ãë·ÖÊ±ÈÕÔÂÖÜÄê ×îµÍÎ»¶ÁÐ´Î»;-------//
//                              Ãë    ·Ö    Ê±    ÈÕ    ÔÂ    ÖÜ    Äê
uchar code READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d}; 
uchar code WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};

//---DS1302Ê±ÖÓ³õÊ¼»¯2021Äê3ÔÂ25ÈÕÐÇÆÚÁù12µã00·Ö00Ãë¡£---//
//---´æ´¢Ë³ÐòÊÇÃë·ÖÊ±ÈÕÔÂÖÜÄê,´æ´¢¸ñÊ½ÊÇÓÃBCDÂë---//
//È«¾Ö±äÁ¿               Ãë    ·Ö    Ê±    ÈÕ    ÔÂ    ÖÜ    Äê            
extern uchar TIME[7] = {0x10, 0x40, 0x23, 0x25, 0x03, 0x06, 0x21};

/*******************************************************************************
* º¯ Êý Ãû         : Ds1302Write
* º¯Êý¹¦ÄÜ		   : ÏòDS1302ÃüÁî£¨µØÖ·+Êý¾Ý£©
* Êä    Èë         : addr,dat
* Êä    ³ö         : ÎÞ
*******************************************************************************/

void Ds1302Write(uchar addr, uchar dat)
{
	uchar n;
	RST = 0;
	_nop_();

	SCLK = 0;//ÏÈ½«SCLKÖÃµÍµçÆ½¡£
	_nop_();
	RST = 1; //È»ºó½«RST(CE)ÖÃ¸ßµçÆ½¡£
	_nop_();

	for (n=0; n<8; n++)//¿ªÊ¼´«ËÍ°ËÎ»µØÖ·ÃüÁî
	{
		DSIO = addr & 0x01;//Êý¾Ý´ÓµÍÎ»¿ªÊ¼´«ËÍ
		addr >>= 1;
		SCLK = 1;//Êý¾ÝÔÚÉÏÉýÑØÊ±£¬DS1302¶ÁÈ¡Êý¾Ý
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)//Ð´Èë8Î»Êý¾Ý
	{
		DSIO = dat & 0x01;
		dat >>= 1;
		SCLK = 1;//Êý¾ÝÔÚÉÏÉýÑØÊ±£¬DS1302¶ÁÈ¡Êý¾Ý
		_nop_();
		SCLK = 0;
		_nop_();	
	}	
		 
	RST = 0;//´«ËÍÊý¾Ý½áÊø
	_nop_();
}

/*******************************************************************************
* º¯ Êý Ãû         : Ds1302Read
* º¯Êý¹¦ÄÜ		   : ¶ÁÈ¡Ò»¸öµØÖ·µÄÊý¾Ý
* Êä    Èë         : addr
* Êä    ³ö         : dat
*******************************************************************************/

uchar Ds1302Read(uchar addr)
{
	uchar n,dat,dat1;
	RST = 0;
	_nop_();

	SCLK = 0;//ÏÈ½«SCLKÖÃµÍµçÆ½¡£
	_nop_();
	RST = 1;//È»ºó½«RST(CE)ÖÃ¸ßµçÆ½¡£
	_nop_();

	for(n=0; n<8; n++)//¿ªÊ¼´«ËÍ°ËÎ»µØÖ·ÃüÁî
	{
		DSIO = addr & 0x01;//Êý¾Ý´ÓµÍÎ»¿ªÊ¼´«ËÍ
		addr >>= 1;
		SCLK = 1;//Êý¾ÝÔÚÉÏÉýÑØÊ±£¬DS1302¶ÁÈ¡Êý¾Ý
		_nop_();
		SCLK = 0;//DS1302ÏÂ½µÑØÊ±£¬·ÅÖÃÊý¾Ý
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)//¶ÁÈ¡8Î»Êý¾Ý
	{
		dat1 = DSIO;//´Ó×îµÍÎ»¿ªÊ¼½ÓÊÕ
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;//DS1302ÏÂ½µÑØÊ±£¬·ÅÖÃÊý¾Ý
		_nop_();
	}

	RST = 0;
	_nop_();	//ÒÔÏÂÎªDS1302¸´Î»µÄÎÈ¶¨Ê±¼ä,±ØÐëµÄ¡£
	SCLK = 1;
	_nop_();
	DSIO = 0;
	_nop_();
	DSIO = 1;
	_nop_();
	return dat;	
}

/*******************************************************************************
* º¯ Êý Ãû         : Ds1302Init
* º¯Êý¹¦ÄÜ		   : ³õÊ¼»¯DS1302.
* Êä    Èë         : ÎÞ
* Êä    ³ö         : ÎÞ
*******************************************************************************/

void Ds1302Init()
{
	uchar n;
	Ds1302Write(0x8E,0X00);		 //½ûÖ¹Ð´±£»¤£¬¾ÍÊÇ¹Ø±ÕÐ´±£»¤¹¦ÄÜ
	for (n=0; n<7; n++)//Ð´Èë7¸ö×Ö½ÚµÄÊ±ÖÓÐÅºÅ£º·ÖÃëÊ±ÈÕÔÂÖÜÄê
	{
		Ds1302Write(WRITE_RTC_ADDR[n],TIME[n]);	
	}
	Ds1302Write(0x8E,0x80);		 //´ò¿ªÐ´±£»¤¹¦ÄÜ
}

/*******************************************************************************
* º¯ Êý Ãû         : Ds1302ReadTime
* º¯Êý¹¦ÄÜ		   : ¶ÁÈ¡Ê±ÖÓÐÅÏ¢
* Êä    Èë         : ÎÞ
* Êä    ³ö         : ÎÞ
*******************************************************************************/

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<7; n++)//¶ÁÈ¡7¸ö×Ö½ÚµÄÊ±ÖÓÐÅºÅ£º·ÖÃëÊ±ÈÕÔÂÖÜÄê
	{
		TIME[n] = Ds1302Read(READ_RTC_ADDR[n]);
	}	
}


//¶¨ÒåIO¿Ú
sbit relay1 = P2^2;  //Ë®Ñ­»·¼ÌµçÆ÷
sbit relay2 = P2^1;	 //¼ÓÈÈ¼ÌµçÆ÷
sbit relay3 = P2^0;	 //³äÑõ¼ÌµçÆ÷

//ÑÓÊ±º¯Êý
void delayms( uint ms )
{
	uchar i;
	while ( ms-- )
	{
		for (i=0;i<120;i++);
	}
}

void main()
{
	Ds1302Init();
	while(1)
	{
		Ds1302ReadTime(); //Ê±ÖÓTIME
		if(TIME[0]==20)
		{
			relay1=0;
			delayms(3000);
		}
		else if(TIME[0]==25)
		{
			relay2=0;
			delayms(3000);
		}
		else if(TIME[0]==33)
		{
			relay3=0;
			delayms(3000);
		}
		else if(TIME[0]==37)
		{
			relay1 = 1;  //Ë®Ñ­»·¼ÌµçÆ÷
			relay2 = 1;	 //¼ÓÈÈ¼ÌµçÆ÷
			relay3 = 1;	 //³äÑõ¼ÌµçÆ
		}
	}
}