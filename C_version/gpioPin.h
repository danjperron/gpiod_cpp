#pragma once

#define PIN_MAX 28
#define OUTPUT 0
#define INPUT 1
#define INPUT_PULLUP 2
#define INPUT_PULLDOWN 3
#define OPENDRAIN_PULLUP 4
#define LOW 0
#define HIGH 1

int pinMode(int pin,int  mode);
void digitalWrite(int pin,int  output);
int digitalRead(int pin);

extern struct gpiod_line  *gpioline[PIN_MAX];
extern int isPinExported(int pin);
int init_gpiod();
void release_gpiod();


