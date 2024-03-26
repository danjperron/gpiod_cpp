
#include <stdio.h>
#include "gpioPin.h"
#include <unistd.h>
#include <signal.h>


// to compile
// gcc -o main main.c gpioPin.c -lgpiod



#define TRUEFALSE(A)  (A? " true" : "false")


int main(void)
{
 // TEST GPIO
 printf("Test GPIO Pin 26 and 26 \n");
 printf("Set Pin26: %s\n",TRUEFALSE(pinMode(26,OUTPUT)));
 digitalWrite(26,1);
 printf("Read 26 :out =1 => %d\n",digitalRead(26));
 digitalWrite(26,0);
 printf("Read 26 :out =0 => %d\n",digitalRead(26));
 printf("Set input 26 pull down: %s\n",TRUEFALSE(pinMode(26,INPUT_PULLDOWN)));
 printf("Read 26 in : %d\n",digitalRead(26));
 printf("Set input 26: %s\n",TRUEFALSE(pinMode(26,INPUT)));
 printf("Read 26 in : %d\n",digitalRead(26));
 printf("Set input pullup 26: %s\n",TRUEFALSE(pinMode(26,INPUT_PULLUP)));
 usleep(100000);
 printf("Read 26 in : %d\n",digitalRead(26));
 release_gpiod();
 return 0;

}

