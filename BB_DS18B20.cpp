// (c) Copywright  Nov 24, 2023  Daniel Perron
//  version using gpiod library
//  extract from DS18B20Pi5Scan.c
//  read from Raspberry Pi 5  GPIO  A DS18B20 sensor by bitbanging 1 wire protocol 
//  from user space

#include "BB_DS18B20.hpp"
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <gpiod.h>
#include <iomanip>



using namespace std;


BB_DS18B20::BB_DS18B20(struct gpiod_line *_gpioline)
    {
       gpioline=_gpioline;
       ScanCount=0;

       ScanID = new unsigned long long[SCAN_MAX];
       for(int loop=0;loop<SCAN_MAX;loop++)
          ScanID[loop]=0LL;
       WaitTime=750;
    }

BB_DS18B20::~BB_DS18B20()
    {
      delete []ScanID;
    }


void BB_DS18B20::clrBit(void)
    {
      gpiod_line_set_value(gpioline,0);
    }

void BB_DS18B20::setBit(void)
    {
      gpiod_line_set_value(gpioline,1);
    }
int  BB_DS18B20::readBit(void)
    {
      return gpiod_line_get_value (gpioline);
    }

int  BB_DS18B20::ReadScratchPad(void)
{
   int loop;
       WriteByte(DS18B20_READ_SCRATCHPAD);
       for(loop=0;loop<9;loop++)
         {
          ScratchPad[loop]=ReadByte();
        }
    return 1;
}

void BB_DS18B20::WriteScratchPad(uint8_t TH, uint8_t TL, uint8_t config)
{

    // First reset device

    DoReset();

    usleep(10);
    // Skip ROM command
     WriteByte(DS18B20_SKIP_ROM);


     // Write Scratch pad

    WriteByte(DS18B20_WRITE_SCRATCHPAD);

    // Write TH

    WriteByte(TH);

    // Write TL

    WriteByte(TL);

    // Write config

    WriteByte(config);
}


void  BB_DS18B20::CopyScratchPad(void)
{

   // Reset device
    DoReset();
    usleep(1000);

   // Skip ROM Command

    WriteByte(DS18B20_SKIP_ROM);

   //  copy scratch pad

    WriteByte(DS18B20_COPY_SCRATCHPAD);
    usleep(100000);
}


void BB_DS18B20::ChangeSensorsResolution(int resolution)
{
   uint8_t config=0;

        switch(resolution)
         {
           case 9:  config=0x1f;break;
           case 10: config=0x3f;break;
           case 11: config=0x5f;break;
           default: config=0x7f;break;
         }
      WriteScratchPad(0xff,0xff,config);
      usleep(1000);
      CopyScratchPad();
}


int BB_DS18B20::GlobalStartConversion(void)
{
   int retry=0;

   while(retry<10)
   {
     if(!DoReset())
      usleep(10000);
     else
      {
       WriteByte(DS18B20_SKIP_ROM);
       WriteByte(DS18B20_CONVERT_T);
       usleep(WaitTime * 1000);
       return 1;
      }
    retry++;
   }
   return 0;


}


void BB_DS18B20::ScanForSensor()
{
  unsigned long long  ID=0ULL;
  int  NextBit=64;
  int  _NextBit;
  int  rcode;
  int retry=0;
  unsigned long long  _ID;
  uint8_t   _ID_CRC;
  uint8_t _ID_Calc_CRC;
  //unsigned char  _ID_Family;
  while(retry<10){
   _ID=ID;
   _NextBit=NextBit;
   rcode=SearchSensor(&_ID,&_NextBit);
    if(rcode==1)
     {
        _ID_CRC =  (uint8_t)  (_ID>>56);
        _ID_Calc_CRC =  CalcCRC((uint8_t *) &_ID, 7);
        if(_ID_CRC == _ID_Calc_CRC)
        {
            ID=_ID;
            ScanID[ScanCount++]=ID;
            NextBit=_NextBit;
        }
        else retry++;
     }
    else if(rcode==0 )
     break;
    else
     retry++;
}
}

/*


void BB_DS18B20::ScanForSensor()
{
  unsigned long long  ID=0ULL;
  int  NextBit=64;
  int  _NextBit;
  int  rcode;
  int retry=0;
  unsigned long long  _ID;
  uint8_t   _ID_CRC;
  uint8_t _ID_Calc_CRC;

  ScanCount=0;

  while(retry<10){
   _ID=ID;
   _NextBit=NextBit;
   rcode=SearchSensor(&_ID,&_NextBit);
    if(rcode==1)
     {
        _ID_CRC =  (uint8_t)  (_ID>>56);
        _ID_Calc_CRC =  CalcCRC((uint8_t *) &_ID, 7);
        if(_ID_CRC == _ID_Calc_CRC)
        {
            ScanID[ScanCount++]=_ID;
            NextBit=_NextBit;
        }
        else retry++;
     }
    else if(rcode==0 )
     break;
    else
     retry++;
}
}

*/


void BB_DS18B20::SelectSensor(unsigned  long long ID)
{
int BitIndex;

WriteByte(DS18B20_MATCH_ROM);

for(BitIndex=0;BitIndex<64;BitIndex++)
   WriteBit(IDGetBit(&ID,BitIndex));

}


int  BB_DS18B20::SearchSensor(unsigned long long * ID, int * LastBitChange)
{
  int BitIndex;
  char Bit , NoBit;


if(*LastBitChange <0) return 0;

// Set bit at LastBitChange Position to 1
// Every bit after LastbitChange will be 0

if(*LastBitChange <64)
{

   IDSetBit(ID,*LastBitChange,1);
   for(BitIndex=*LastBitChange+1;BitIndex<64;BitIndex++)
    IDSetBit(ID,BitIndex,0);
}

*LastBitChange=-1;

if(!DoReset()) return -1;


WriteByte(DS18B20_SEARCH_ROM);

  for(BitIndex=0;BitIndex<64;BitIndex++)
    {

      NoBit = ReadBit();
      Bit = ReadBit();

     if(Bit && NoBit)
        return -2;

     if(!Bit && !NoBit)
        {
          // ok 2 possibilities
//          printf("B");
          if(IDGetBit(ID,BitIndex))
            {
               // Bit High already set 
                WriteBit(1);
             }
          else
             {
               // ok let's try LOW value first
               *LastBitChange=BitIndex;
                WriteBit(0);
             }
         }
      else if(!Bit)
        { 
//	printf("1");
         WriteBit(1);
         IDSetBit(ID,BitIndex,1);
        }
      else
        {
        //printf("0");
        WriteBit(0);
        IDSetBit(ID,BitIndex,0);
        }
//   if((BitIndex % 4)==3)printf(" ");
    }
//
// printf("\n");
  return 1;



}



int BB_DS18B20::ReadSensor(unsigned long long ID)
{
  int RetryCount;
 // int loop;
  unsigned char  CRCByte;
  union {
   short SHORT;
   unsigned char CHAR[2];
  }IntTemp;

  temperature=-9999.9;

  for(RetryCount=0;RetryCount<10;RetryCount++)
  {

   if(!DoReset()) continue;

   // start a conversion
   SelectSensor(ID);

  if(!ReadScratchPad()) continue;

  // OK Check sum Check;
  CRCByte= CalcCRC(ScratchPad,(uint8_t)8);

  if(CRCByte!=ScratchPad[8]) continue;;

  //Check Resolution
   resolution=0;
   switch(ScratchPad[4])
   {
     case  0x1f: resolution=9;break;
     case  0x3f: resolution=10;break;
     case  0x5f: resolution=11;break;
     case  0x7f: resolution=12;break;
   }

   if(resolution==0) continue;
    // Read Temperature

    IntTemp.CHAR[0]=ScratchPad[0];
    IntTemp.CHAR[1]=ScratchPad[1];

    temperature =  0.0625 * (double) IntTemp.SHORT;
    return 1;
    }
  return 0;
}

void BB_DS18B20::Delay1us(void)
 {
   DelayNanosecondsNoSleep(1000);
 }

void BB_DS18B20::DelayNanosecondsNoSleep (int delay_ns)
{
   long int start_time;
   long int time_difference;
   struct timespec gettime_now;

   clock_gettime(CLOCK_REALTIME, &gettime_now);
   start_time = gettime_now.tv_nsec;      //Get nS value
   while (1)
   {
      clock_gettime(CLOCK_REALTIME, &gettime_now);
      time_difference = gettime_now.tv_nsec - start_time;
      if (time_difference < 0)
         time_difference += 1000000000;            //(Rolls over every 1 second)
      if (time_difference > (delay_ns ))      //Delay for # nS
         break;
   }
}

void BB_DS18B20::DelayMicrosecondsNoSleep (int delay_us)
{
   long int start_time;
   long int time_difference;
   struct timespec gettime_now;

   clock_gettime(CLOCK_REALTIME, &gettime_now);
   start_time = gettime_now.tv_nsec;      //Get nS value
   while (1)
   {
      clock_gettime(CLOCK_REALTIME, &gettime_now);
      time_difference = gettime_now.tv_nsec - start_time;
      if (time_difference < 0)
         time_difference += 1000000000;            //(Rolls over every 1 second)
      if (time_difference > (delay_us * 1000))      //Delay for # nS
         break;
   }
}

int  BB_DS18B20::DoReset(void)
{

   setBit();
   DelayMicrosecondsNoSleep(10);
   clrBit();
   DelayMicrosecondsNoSleep(480);
   setBit();
   DelayMicrosecondsNoSleep(60);
   if(readBit()==0)
   {
     DelayMicrosecondsNoSleep(420);
     return 1;
   }
  return 0;
}

void BB_DS18B20::WriteByte(uint8_t value)
{
  uint8_t Mask=1;
  int loop;

   for(loop=0;loop<8;loop++)
     {
       clrBit();

       if((value & Mask)!=0)
        {
	   Delay1us();
           setBit();
           DelayMicrosecondsNoSleep(60);

        }
        else
        {
           DelayMicrosecondsNoSleep(60);
           setBit();
           DelayMicrosecondsNoSleep(1);
        }
      Mask*=2;
      DelayMicrosecondsNoSleep(60);
    }
   usleep(100);
}

void BB_DS18B20::WriteBit(uint8_t value)
{
   clrBit();
   if(value)
    {
      Delay1us();
      setBit();
      DelayMicrosecondsNoSleep(60);
    }
   else
    {
      DelayMicrosecondsNoSleep(60);
      setBit();
      DelayMicrosecondsNoSleep(1);
     }
   DelayMicrosecondsNoSleep(60);
}

uint8_t BB_DS18B20::ReadBit(void)
{
   unsigned char rvalue=0;
   // PIN LOW
   clrBit();
   Delay1us();
   // set INPUT
   setBit();
   DelayMicrosecondsNoSleep(2);
   if(readBit()!=0)
    rvalue=1;
   DelayMicrosecondsNoSleep(60);
   return rvalue;
}

uint8_t BB_DS18B20::ReadByte(void)
{

   uint8_t  Mask=1;
   int loop;
   uint8_t data=0;


   for(loop=0;loop<8;loop++)
     {
       //  set output
       clrBit();
       //  PIN LOW
       Delay1us();
       //  set input
       setBit();
       // Wait  2 us
       DelayMicrosecondsNoSleep(2);
       if(readBit()!=0)
       data |= Mask;
       Mask*=2;
       DelayMicrosecondsNoSleep(60);
      }

    return data;
}


uint8_t  BB_DS18B20::CalcCRC(uint8_t * data, int byteSize)
{
   uint8_t   shift_register = 0;
   unsigned char  loop,loop2;
   uint8_t   DataByte;

   for(loop = 0; loop < byteSize; loop++)
   {
      DataByte = *(data + loop);
      for(loop2 = 0; loop2 < 8; loop2++)
      {
         if((shift_register ^ DataByte)& 1)
         {
            shift_register = shift_register >> 1;
            shift_register ^=  0x8C;
         }
         else
            shift_register = shift_register >> 1;
         DataByte = DataByte >> 1;
      }
   }
   return shift_register;
}

uint8_t BB_DS18B20::IDGetBit(unsigned long long *llvalue,uint8_t bit)
{
  unsigned long long Mask = 1ULL << bit;

  return ((*llvalue & Mask) ? 1 : 0);
}



unsigned long long   BB_DS18B20::IDSetBit(unsigned long long *llvalue,uint8_t bit, uint8_t newValue)
{
  unsigned long long Mask = 1ULL << bit;

  if((bit >= 0) && (bit < 64))
  {
  if(newValue==0)
   *llvalue &= ~Mask;
  else
   *llvalue |= Mask;
   }
  return *llvalue;
}


string BB_DS18B20::IdToString(unsigned long long id)
{
    string SensorID="";
    stringstream ss;
    std::ios_base::fmtflags f( ss.flags() );

    ss << uppercase << hex << setw(2) << setfill('0') << (id & 0xff) ;
    ss << "-";
    ss << uppercase << hex << setw(14) << setfill('0') << (id >>8) ;
    ss >> SensorID;
    ss.flags( f );
    return SensorID;
}


