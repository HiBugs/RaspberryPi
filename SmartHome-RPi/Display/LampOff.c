#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include "io.h"

int main()
{
	FILE *fp;
	wiringPiSetup(); 
	pinMode(Lamp_IO, OUTPUT);
	digitalWrite(Lamp_IO,1);
	if(digitalRead(Lamp_IO)==1){
		fp=fopen("Lamp.txt","wb");
		fprintf(fp,"0");
		fclose(fp);
	}
	return 0;
}