#define MAX_TIME 85
#define ATTEMPTS 5  

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include "io.h"
extern int dht11_val[5];
int dht11_read_val(){
    uint8_t lststate=HIGH;         
    uint8_t counter=0;
    uint8_t j=0,i;
    for(i=0;i<5;i++)
        dht11_val[i]=0;
         
    //host send start signal    
    pinMode(DHT11PIN,OUTPUT);      
    digitalWrite(DHT11PIN,LOW);    
    delay(18);
    digitalWrite(DHT11PIN,HIGH);   
    delayMicroseconds(40);
     
    //start recieve dht response
    pinMode(DHT11PIN,INPUT);       
    for(i=0;i<MAX_TIME;i++)         
    {
        counter=0;
        while(digitalRead(DHT11PIN)==lststate){     
            counter++;
            delayMicroseconds(1);
            if(counter==255)
                break;
        }
        lststate=digitalRead(DHT11PIN);             
        if(counter==255)                           
            break;
        // top 3 transistions are ignored, maybe aim to wait for dht finish response signal
        if((i>=4)&&(i%2==0)){
            dht11_val[j/8]<<=1;                     
            if(counter>16)                          
                dht11_val[j/8]|=1;                   
            j++;
        }
    }
    // verify checksum and print the verified data
    if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF))){
        printf("RH:%d,TEMP:%d\n",dht11_val[0],dht11_val[2]);
        return 1;
    }
    else
        return 0;
}