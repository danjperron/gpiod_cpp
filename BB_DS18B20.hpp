// (c) Copywright  Nov 24, 2023  Daniel Perron
//  version using gpiod library
//  DS18B20Pi5Scan.c
//  read from Raspberry Pi 5  GPIO  A DS18B20 sensor by bitbanging 1 wire protocol 
//  from user space

#include <cstdint>
#include <string>


class BB_DS18B20
{
  public:
          BB_DS18B20(struct gpiod_line * _gpioline);
          ~BB_DS18B20();
          int  ReadScratchPad(void);
          void WriteScratchPad(uint8_t TH, uint8_t TL, uint8_t config);
          void  CopyScratchPad(void);
          void ChangeSensorsResolution(int resolution);
          int GlobalStartConversion(void);
          void ScanForSensor();
          void SelectSensor(unsigned  long long ID);
          int  SearchSensor(unsigned long long * ID, int * LastBitChange);
          int ReadSensor(unsigned long long ID);

          int ScanCount;
          const int SCAN_MAX=64;
          unsigned long long *ScanID;
	  double  	temperature;
	  int           resolution;
          std::string  IdToString(unsigned long long id);
  private:
          struct gpiod_line * gpioline;
          void clrBit(void);
          void setBit(void);
          int  readBit(void);
          void Delay1us(void);
          void DelayNanosecondsNoSleep (int delay_ns);
          void DelayMicrosecondsNoSleep (int delay_us);
          int DoReset(void);
          void WriteByte(uint8_t value);
          void WriteBit(uint8_t value);
          uint8_t ReadBit(void);
          uint8_t ReadByte(void);
          uint8_t CalcCRC(uint8_t * data, int  byteSize);
          uint8_t IDGetBit(unsigned long long *llvalue, uint8_t  bit);
          unsigned long long   IDSetBit(unsigned long long *llvalue,uint8_t bit, uint8_t newValue);

	  const uint8_t DS18B20_SKIP_ROM=0xCC;
          const uint8_t DS18B20_CONVERT_T=0x44;
          const uint8_t DS18B20_MATCH_ROM=0x55;
	  const uint8_t DS18B20_SEARCH_ROM=0XF0;
	  const uint8_t DS18B20_READ_SCRATCHPAD=0xBE;
          const uint8_t DS18B20_WRITE_SCRATCHPAD=0x4E;
          const uint8_t DS18B20_COPY_SCRATCHPAD=0x48;
          uint8_t	ScratchPad[9];
          int           WaitTime;

};




