#pragma once

const int PIN_MAX=28;
const int OUTPUT=0;
const int INPUT=1;
const int INPUT_PULLUP=2;
const int INPUT_PULLDOWN=3;
const int OPENDRAIN_PULLUP=4 ;
const int LOW=0;
const int HIGH=1;
bool pinMode(int pin,int  mode);
void digitalWrite(int pin,int  output);
int digitalRead(int pin);

extern struct gpiod_line  *gpioline[PIN_MAX];
extern bool isPinExported(int pin);
bool init_gpiod();
void release_gpiod();


