#include<stdio.h>
#include<string.h>
#define u32  unsigned long
#define u16  unsigned int
#define u8	unsigned char

#define LCD_X_Parameter		128	   			//LCD宽度
#define LCD_Y_Parameter		64			  	//LCD高度
#define ZK_InitAddr       	0x00000
#define ASCII_InitAdd		0x8100

#define Lcd_SDA   5	//SDI
#define Lcd_CLK   4	//CLK
#define Lcd_RD    1	//D/C
#define Lcd_CS    0	//CS1

#define FLASH_CS  3	//CS2
#define FLASH_SI  2	//FSO


void SPI_FLASH_CS(u8 value)
{
    digitalWrite(FLASH_CS,value);
}

void SPI_FLASH_SO(u8 value)
{
    digitalWrite(Lcd_SDA,value);
}

void SPI_FLASH_CLK(u8 value)
{
    digitalWrite(Lcd_CLK,value);
}

void FlashInit(void)
{
	SPI_FLASH_CS(1);
	SPI_FLASH_CLK(1);
}

void LCD_CS(u8 value)	  
{
	 digitalWrite(Lcd_CS,value);
}
void LCD_RD(u8 value)
{
	digitalWrite(Lcd_RD,value);
}
void LCD_CLK(u8 value)
{
	digitalWrite(Lcd_CLK,value);
}
void LCD_SDA(u8 value)
{
	digitalWrite(Lcd_SDA,value);
}


u8 Write_8bit_FLASH(u8 value)
{
   u8 i;
   u8 temp=0;
   SPI_FLASH_CLK(1);
   for(i=0;i<8;i++)
   {
	   SPI_FLASH_CLK(0);
	   delayMicroseconds(2);
	   if((value&0x80)==0x80)
	       SPI_FLASH_SO(1);
	   else
	       SPI_FLASH_SO(0);
	   value<<=1;
	   delayMicroseconds(2);
 	   SPI_FLASH_CLK(1);
	   delayMicroseconds(2);
	   temp<<=1;
	   if(digitalRead(FLASH_SI)==1)
	      temp++;
   }
   return(temp);
}

void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	LCD_CS(1);
	SPI_FLASH_CS(0);
	Write_8bit_FLASH(0x03);
	Write_8bit_FLASH((ReadAddr & 0xFF0000) >> 16);
	Write_8bit_FLASH((ReadAddr& 0xFF00) >> 8);
	Write_8bit_FLASH(ReadAddr & 0xFF);
	
	while(NumByteToRead--)
	{
		*pBuffer = Write_8bit_FLASH(0xA5);
		pBuffer++;
	}
	
	SPI_FLASH_CS(1);
	LCD_CS(0);
}


void LCD_WData(u8 value)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		if((value << i) & 0x80)
		{
			LCD_SDA(1);
		}
		else
			LCD_SDA(0);
		LCD_CLK(0);
		LCD_CLK(1);
	}
}
void TransData(u8 value,u8 RD)
{
	LCD_RD(RD);
	LCD_WData(value);
}	

void PageSet(u8 page,u8 column)//page = y,column = x
{
	TransData(0xb0+page,0);
	TransData(((column&0xf0)>>4)|0x10,0);
	TransData((column&0x0f)|0x01,0);
}

void LCDFill(unsigned char bmp_dat)
{
	unsigned char y,x;
	
	LCD_CS(0);
	for(y=0;y<8;y++)
	{
		TransData(0xb0+y,0);
		TransData(0x01,0);
		TransData(0x10,0);
		for(x=0;x<LCD_X_Parameter;x++)
		TransData(bmp_dat,1);
	}
	LCD_CS(1);
}
void Lcdclear(void)
{
	LCDFill(0x00);
}

void Lcd_Init(void)
{
	delay(500);
	
	LCD_CS(0);
	TransData(0xae,0);
	TransData(0x00,0);
	TransData(0x10,0);
	TransData(0x40,0);
	TransData(0x81,0);
	TransData(0xcf,0);
	TransData(0xa1,0);
	TransData(0xc8,0);
	TransData(0xa6,0);
	TransData(0xa8,0);
	TransData(0x3f,0);
	TransData(0xd3,0);
	TransData(0x00,0);
	TransData(0xd5,0);
	TransData(0x80,0);
	TransData(0xd9,0);
	TransData(0xf1,0);
	TransData(0xda,0);
	TransData(0x12,0);
	TransData(0xdb,0);
	TransData(0x40,0);
	TransData(0x20,0);
	TransData(0x02,0);
	TransData(0x8d,0);
	TransData(0x14,0);
	TransData(0xa4,0);
	TransData(0xa6,0);
	TransData(0xaf,0);
	LCD_CS(1);
	
	Lcdclear();//清除显示
}

void LcdDisChar(u8 xPos,u8 yPos,u8 zknum,u8 *zkzip)
{
	u8 i;
	PageSet(yPos,xPos);
	for(i=0; i<zknum;i++)
	{
		TransData(zkzip[i],1);
	}
	PageSet(yPos+1,xPos);
	for(i=zknum; i<zknum*2;i++)
	{
		TransData(zkzip[i],1);
	}
}

/*******************************************************************************
* 功能           : 调取单个汉字字库显示汉字或字符函数
* Description    : 
* Input          : xPos ：      Lcd 横向地址0-128
                   yPos ：      Lcd 纵向地址0-64
* GBCodeptr      ：单个汉字或ASCII字符的字符串。
* Output         : None	
* Return         : 0：Lcd设置超出。1：设置正确。
*******************************************************************************/

u8 LcdDisplay_HZ(u8 xPos,u8 yPos,u8 *GBCodeptr)
{
	u8 msb,lsb,zknum;
	u8 zkzip[32];  //读取字库数据的缓存区
	u32 offset;	   //字库地址索引
	
	LCD_CS(0);
	if(xPos>=LCD_X_Parameter || yPos>=LCD_Y_Parameter) 
		return 0 ;  //超范围退出
	msb= *GBCodeptr;     //汉字或ASCII的机器码的低8位。
	lsb= *(GBCodeptr+1); //汉字或ASCII的机器码的高8位。
	if (msb>128 && lsb>128)	//表明为汉字
	{
		if(xPos+16>LCD_X_Parameter || yPos+16>LCD_Y_Parameter)return 0; //超范围退出
		offset =ZK_InitAddr+((u32)((msb-0xA1)*94+(lsb-0xa1))*32);//具体算法详细查看字库原理
		zknum =16;	//汉字为16*16的字库
	}
	else	        //否则为ASCII码
	{
		if(xPos+8>LCD_X_Parameter || yPos+16>LCD_Y_Parameter)return 0;	//超范围退出
		offset =ASCII_InitAdd+(msb-32)*16;  //查看提供的2012_KZ.txt文档中的“!”的首地址
		zknum =8;   // ASCII码位8*16的字库
	}
	SPI_FLASH_BufferRead(zkzip,offset,zknum*2);	 //从FLASH中读取字库数据。
	LcdDisChar(xPos,yPos,zknum,zkzip);
	LCD_CS(1);

	return 1;
}

/*******************************************************************************
* 功能           : 显示多个汉字或字符的函数
* Description    : 
* Input          : xPos ：      Lcd 横向地址0-128   
                   yPos ：      Lcd 纵向地址0-64
* GBCodeptr      ：多个汉字的字符串。
* Output         : None	
* Return         : None
*******************************************************************************/
void LcdDisplay_Chinese(u8 xPos,u8 yPos,u8 *GBCodeptr)
{
	u8 i, len;
	len =  strlen((const char*)GBCodeptr);
	printf("%d\n",len);
	for(i=0;i<len;i++)
	{
		LcdDisplay_HZ(xPos+i*8,yPos,GBCodeptr+i);
		printf("%c",*(GBCodeptr+1));
		i++;
	}
}

/*******************************************************************************
* 功能           : 显示多个汉字或字符的函数
* Description    : 
* Input          : xPos ：      Lcd 横向地址0-127
                   yPos ：      Lcd 纵向地址0-7
* GBCodeptr      ：多个ASCII字符的字符串。
* Output         : None	
* Return         : None
*******************************************************************************/
void LcdDisplay_char(u8 xPos,u8 yPos,u8 *GBCodeptr)
{
	u8 i, len;
	len =  strlen((const char*)GBCodeptr);
	for(i=0;i<len;i++)
	{
		LcdDisplay_HZ(xPos+i*8,yPos,GBCodeptr+i);
	}
}
