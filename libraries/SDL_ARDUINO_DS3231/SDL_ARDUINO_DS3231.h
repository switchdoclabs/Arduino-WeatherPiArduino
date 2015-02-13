/*
 * SDL_ARDUINO_DS3231.h - library for DS3231 RTC
 * This library is intended to be uses with Arduino Time.h library functions
 */

#ifndef DS3231_h
#define DS3231_h


#include <Time.h>

// library interface description
class DS3231
{
    // user-accessible "public" interface
public:
    DS3231();
    static time_t get();
    static bool set(time_t t);
    static bool read(tmElements_t &tm);
    static bool write(tmElements_t &tm);
    static bool chipPresent() { return exists; }
    static float get_treg(void);
    
private:
    static bool exists;
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

extern DS3231 RTC;

#endif


