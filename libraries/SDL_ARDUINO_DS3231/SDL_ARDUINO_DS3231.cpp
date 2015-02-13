/*
 * SDL_ARDUINO_DS3231.h - library for DS3231 RTC
 
 Derived from:
 ////////////////
 Copyright (c) Michael Margolis 2009
 This library is intended to be uses with Arduino Time.h library functions
 
 The library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 30 Dec 2009 - Initial release
 5 Sep 2011 updated for Arduino 1.0
 /////////////
 
 14 January 2014 - Updated for DS3231
 */

#include "Arduino.h"
#include <Wire.h>
#include "SDL_ARDUINO_DS3231.h"

#define DS3231_CTRL_ID 0x68

DS3231::DS3231()
{
    Wire.begin();
}

// PUBLIC FUNCTIONS
time_t DS3231::get()   // Aquire data from buffer and convert to time_t
{
    tmElements_t tm;
    if (read(tm) == false) return 0;
    return(makeTime(tm));
}

bool DS3231::set(time_t t)
{
    tmElements_t tm;
    breakTime(t, tm);
    tm.Second |= 0x80;  // stop the clock
    write(tm);
    tm.Second &= 0x7f;  // start the clock
    write(tm);
}

// Aquire data from the RTC chip in BCD format
bool DS3231::read(tmElements_t &tm)
{
    uint8_t sec;
    Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
    Wire.write((uint8_t)0x00);
#else
    Wire.send(0x00);
#endif
    if (Wire.endTransmission() != 0) {
        exists = false;
        return false;
    }
    exists = true;
    
    // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
    Wire.requestFrom(DS3231_CTRL_ID, tmNbrFields);
    if (Wire.available() < tmNbrFields) return false;
#if ARDUINO >= 100
    sec = Wire.read();
    tm.Second = bcd2dec(sec & 0x7f);
    tm.Minute = bcd2dec(Wire.read() );
    tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
    tm.Wday = bcd2dec(Wire.read() );
    tm.Day = bcd2dec(Wire.read() );
    tm.Month = bcd2dec(Wire.read() );
    tm.Year = y2kYearToTm((bcd2dec(Wire.read())));
#else
    sec = Wire.receive();
    tm.Second = bcd2dec(sec & 0x7f);
    tm.Minute = bcd2dec(Wire.receive() );
    tm.Hour =   bcd2dec(Wire.receive() & 0x3f);  // mask assumes 24hr clock
    tm.Wday = bcd2dec(Wire.receive() );
    tm.Day = bcd2dec(Wire.receive() );
    tm.Month = bcd2dec(Wire.receive() );
    tm.Year = y2kYearToTm((bcd2dec(Wire.receive())));
#endif
    if (sec & 0x80) return false; // clock is halted
    return true;
}

bool DS3231::write(tmElements_t &tm)
{
    Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
    Wire.write((uint8_t)0x00); // reset register pointer
    Wire.write(dec2bcd(tm.Second)) ;
    Wire.write(dec2bcd(tm.Minute));
    Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format
    Wire.write(dec2bcd(tm.Wday));
    Wire.write(dec2bcd(tm.Day));
    Wire.write(dec2bcd(tm.Month));
    Wire.write(dec2bcd(tmYearToY2k(tm.Year)));
#else
    Wire.send(0x00); // reset register pointer
    Wire.send(dec2bcd(tm.Second)) ;
    Wire.send(dec2bcd(tm.Minute));
    Wire.send(dec2bcd(tm.Hour));      // sets 24 hour format
    Wire.send(dec2bcd(tm.Wday));
    Wire.send(dec2bcd(tm.Day));
    Wire.send(dec2bcd(tm.Month));
    Wire.send(dec2bcd(tmYearToY2k(tm.Year)));
#endif
    if (Wire.endTransmission() != 0) {
        exists = false;
        return false;
    }
    exists = true;
    return true;
}



// temperature register

float DS3231::get_treg()
{
    float rv;
    uint8_t temp_msb, temp_lsb;
    int8_t nint;
    
    Wire.beginTransmission(DS3231_CTRL_ID);
    Wire.write(0x11);
    Wire.endTransmission();
    
    Wire.requestFrom(DS3231_CTRL_ID, 2);
    temp_msb = Wire.read();
    temp_lsb = Wire.read() >> 6;
    
    if ((temp_msb & 0x80) != 0)
        nint = temp_msb | ~((1 << 8) - 1);      // if negative get two's complement
    else
        nint = temp_msb;
    
    rv = 0.25 * temp_lsb + nint;
    
    return rv;
}

// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS3231::dec2bcd(uint8_t num)
{
    return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS3231::bcd2dec(uint8_t num)
{
    return ((num/16 * 10) + (num % 16));
}


bool DS3231::exists = false;

DS3231 RTC = DS3231(); // create an instance for the user

