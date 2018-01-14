#ifndef OLED_H
#define OLED_H

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include "io.h"

int dht11_read_val();
extern int dht11_val[5];
#endif