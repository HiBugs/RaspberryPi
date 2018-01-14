#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include "oled.h"
#include "dht11.h"
#include "io.h"


int flag_HW=0;
int dht11_val[5]={0,0,0,0,0};

void GPIO_Init()
{
	wiringPiSetup(); 
	pinMode(Lcd_SDA, OUTPUT);
	pinMode(Lcd_CLK, OUTPUT);
	pinMode(Lcd_RD, OUTPUT);
	pinMode(Lcd_CS, OUTPUT);
	pinMode(FLASH_CS, OUTPUT);
	pinMode(FLASH_SI, INPUT);
	
	pinMode(FMQ_IO,OUTPUT);
	pinMode(HW_IO,INPUT);
	pinMode(Lamp_IO,OUTPUT);

	digitalWrite(FMQ_IO,1);
	digitalWrite(Lamp_IO,1);
}
void DHT11show(char *Tem,char *Hum){
	int success;     	
	while(1){
		success = dht11_read_val();
		if (success) {                      
			Hum[0]=(char)(dht11_val[0]/10+'0');
			Hum[1]=(char)(dht11_val[0]%10+'0');
			Tem[0]=(char)(dht11_val[2]/10+'0');
			Tem[1]=(char)(dht11_val[2]%10+'0');
			Hum[2]=' ';
			Hum[3]='%';
			Hum[4]='\0';
			break;
		}
		delay(700);
	}
}

void display_wsd()
{
	FILE *fp;
	char Tem[20]={'\0'};
	char Hum[20]={'\0'};
	memset(Tem,'\0',sizeof(Tem));
	memset(Hum,'\0',sizeof(Hum));
	LcdDisplay_Chinese(0,4,"室内温度: ");
	LcdDisplay_Chinese(0,6,"室内湿度: ");
	DHT11show(Tem,Hum);
	fp=fopen("TAH.txt","wb");
	fprintf(fp,"Wendu: %s Shidu: %s",Tem,Hum);
	fclose(fp);
	LcdDisplay_char(85,4,Tem);
	LcdDisplay_char(85,6,Hum);
	LcdDisplay_Chinese(105,4,"℃");
}
void FMQ_display()
{
	flag_HW++;
	if(digitalRead(HW_IO)==1 && flag_HW>10){
		flag_HW=0;
		system("sudo ./CameraPhoto.sh");
		digitalWrite(FMQ_IO,0);
		delay(2000);
		digitalWrite(FMQ_IO,1);
	}
	//printf("hw: %d\n",flag_HW);

}
int main() 
{	
	int n=0;
	GPIO_Init();
	Lcd_Init();
	//Lcdclear();


	LcdDisplay_Chinese(30,0,"温州大学");
	while(1){
		if(n==0 || n>100){
			display_wsd();
			n=0;
		}
		wiringPiISR(HW_IO,INT_EDGE_RISING,FMQ_display);
		delay(500);
		n++;
		/*printf("%d: ",n);
		printf("%d\n",digitalRead(HW_IO));*/
		
	}	
	
	return 0;
}
