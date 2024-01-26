
#include <string>
#include <iostream>
#include <iomanip>
#include "gpioPin.hpp"
#include <unistd.h>
#include <thread>
#include <signal.h>

#include "BB_DS18B20.hpp"

using namespace std;

bool RunThread=true;

void convertLittleToBig(const uint8_t* in, uint8_t* out, int sizeInBytes) {
  for(int i=0;i<sizeInBytes;++i)
    out[i] = in[sizeInBytes-i-1];
}


void pulse19()
{
    while(RunThread)
    {
      digitalWrite(19, digitalRead(19) ? 0 : 1);
      //std::cout << "DigitalRead" << digitalRead(19) << endl;
      usleep(1000000);
    }
    digitalWrite(19,0);
    //std::cout << "Pulse Done ! DigitalRead :" << digitalRead(19)<< endl;
}

void MyExit()
{
 RunThread=false;
 cout << "exit! " << endl;
}

void my_ctrl_c_handler(int s){
           printf("Caught signal %d\n",s);
           RunThread=false;
}





int main(void)
{
 thread thread1;
 std::atexit(MyExit);
 signal (SIGINT,my_ctrl_c_handler);



 // TEST GPIO
 cout << " test GPIO Pin 19 and 26 " << endl;
 cout << "set Pin26: " << pinMode(26,OUTPUT) << endl;
 digitalWrite(26,1);
 cout << "read 26 :out =1 => " << digitalRead(26) << endl;
 digitalWrite(26,0);
 cout << "read 26 :out =0 => " << digitalRead(26) << endl;
 cout << "set input 19 pull down:" << pinMode(19,INPUT_PULLDOWN) <<endl;
 cout << "read 19 in " << digitalRead(19) << endl;
 cout << "set input 26:" << pinMode(26,INPUT) <<endl;
 cout << "read 26 in " << digitalRead(26) << endl;
 cout << "set input pullup 19:" << pinMode(19,INPUT_PULLUP) <<endl;
 usleep(100000);
 cout << "read 19 in " << digitalRead(19) << endl;

for(auto sensor: {4,17})
{

 cout << endl << endl  << "Scan DS18B20 sensors on GPIO" << sensor << endl;
// tester le ds18B20
// PS dtoverlay gpio-w1 doit être enlevé dans /boot/config.txt
// utilisation du bitbanging
pinMode(sensor,OPENDRAIN_PULLUP);
BB_DS18B20 ds18b20(gpioline[sensor]);

ds18b20.ScanForSensor();
cout << "ScanCount :" << ds18b20.ScanCount <<endl;

for(int i=0;i<ds18b20.ScanCount;i++)
{
   cout << ds18b20.IdToString(ds18b20.ScanID[i]) << " : ";
   if(ds18b20.ReadSensor(ds18b20.ScanID[i]))
    {
     cout << ds18b20.resolution << " bits   temperature : ";
     cout << setw(7) <<fixed << setprecision(2) <<   ds18b20.temperature  << "'C" << endl;
    }
   else
    cout << "Unable to read sensor" << endl;
}
}

 cout <<  endl <<  "pulse GPIO 19. Press ctr-l to end! " << endl;
 // pulse  gpio19
 pinMode(19,OUTPUT);
 thread1=thread(pulse19);

 thread1.join();

}

