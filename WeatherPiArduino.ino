// Filename WeatherPiArduino.ino
// Version 1.1 01/15/2014
// SwitchDoc Labs, LLC
//
// Version 1.6 - Added support for ADS1015 and for MOD-1016 Lightning Detector
//
// From:
// Filename ArduinoConnectServer.ino
// Version 1.5 08/15/13 RV MiloCreek
//


//#define WEBDUINO_SERIAL_DEBUGGING  1

#define ETHERNET


#include <SPI.h>

#ifdef ETHERNET
#include "Ethernet.h"
#endif



#define DEBUG


#include "WebServer.h"
#include "MemoryFree.h"
#include "avr/pgmspace.h"

#include "elapsedMillis.h"

#include "SDL_RasPiGraphLibrary.h"

#include "Config.h"


#include <Time.h>
#include <Wire.h>



long messageCount;

static uint8_t mac[] = LOCALMAC;
static uint8_t ip[] = LOCALIP;

// this is our current command object structure.  It is only valid inside void jsonCmd 
typedef struct {
    char ObjectID[40];
    char ObjectFlags[40];
    char ObjectAction[40]; 
    char ObjectName[40];
    char ObjectServerID[40];
    char Password[40];
    char ObjectType[40];
    char Validate[40]; 
} currentObjectStructure;
  
char *md5str;

char ST1Text[40];   // used in ST-1 Send text control


char bubbleStatus[40];   // What to send to the Bubble status

char windSpeedBuffer[150];  // wind speed graph
char windGustBuffer[150];  // wind speed graph
char windDirectionBuffer[150];  // wind speed graph

float windSpeedMin;
float windSpeedMax;
float windGustMin;
float windGustMax;
float windDirectionMin;
float windDirectionMax;


float currentWindSpeed;
float currentWindGust;

float rainTotal;

// weather

#define WpinLED     13   // LED connected to digital pin 13
#define WpinAnem    18  // Anenometer connected to pin 18 - Int 5
#define WpinRain    2   // Rain Bucket connecto to pin 19 - Int 4
#define WintAnem    5
#define WintRain    0

// for mega, have to use Port B - only Port B works.
/*
 Arduino Pins         PORT
 ------------         ----
 Digital 0-7          D
 Digital 8-13         B
 Analog  0-5          C
*/




#include "SDL_Weather_80422.h"


SDL_Weather_80422 weatherStation(WpinAnem, WpinRain, WintAnem, WintRain, A0, SDL_MODE_I2C_ADS1015);

// Mod-1016 - AS3935 Lightning Detector


#include "I2C.h"
#include <AS3935.h>

void printAS3935Registers();


// Iterrupt handler for AS3935 irqs
// and flag variable that indicates interrupt has been triggered
// Variables that get changed in interrupt routines need to be declared volatile
// otherwise compiler can optimize them away, assuming they never get changed
void AS3935Irq();
volatile int AS3935IrqTriggered;

// Library object initialization First argument is interrupt pin, second is device I2C address
AS3935 AS3935(3,0x03);



void printAS3935Registers()
{
  int noiseFloor = AS3935.getNoiseFloor();
  int spikeRejection = AS3935.getSpikeRejection();
  int watchdogThreshold = AS3935.getWatchdogThreshold();
  Serial.print("Noise floor is: ");
  Serial.println(noiseFloor,DEC);
  Serial.print("Spike rejection is: ");
  Serial.println(spikeRejection,DEC);
  Serial.print("Watchdog threshold is: ");
  Serial.println(watchdogThreshold,DEC);  
}

// this is irq handler for AS3935 interrupts, has to return void and take no arguments
// always make code in interrupt handlers fast and short
void AS3935Irq()
{
  AS3935IrqTriggered = 1;
}

// RasPiConnect


#include "MD5.h"

#include "smallJSON.h"

#include "ExecuteJSONCommand.h"

// DS3231 Library functions

#include "SDL_ARDUINO_DS3231.h"

  const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void printDigits(int digits){
  // utility function for digital clock display: prints an leading 0

  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void return2Digits(char returnString[], char *buffer2, int digits)
{
  if(digits < 10)
    sprintf(returnString,"0%i", digits);
  else
    sprintf(returnString,"%i", digits);
    
  strcpy(returnString, buffer2);
    

}

void buildTimeString(char returnString[], char *buffer2, tmElements_t convertTime)
{
  
   
    
    char myBuffer[5];
    sprintf(myBuffer,"%i-", tmYearToCalendar(convertTime.Year));
    strcat(returnString, myBuffer);

    return2Digits(myBuffer, myBuffer, convertTime.Month);
    strcat(returnString, myBuffer);
    strcat(returnString, "-");
 
    return2Digits(myBuffer, myBuffer, convertTime.Day);
    strcat(returnString, myBuffer);
    strcat(returnString, " ");
    
    return2Digits(myBuffer, myBuffer, convertTime.Hour);
    strcat(returnString, myBuffer);
    strcat(returnString, ":");
    
    return2Digits(myBuffer, myBuffer, convertTime.Minute);
    strcat(returnString, myBuffer);
    strcat(returnString, ":");
    
    return2Digits(myBuffer, myBuffer, convertTime.Second);
    strcat(returnString, myBuffer);
  
  
  
}

// AT24C32 EEPROM

#include "AT24C32.h"

// Adafruit FRAM

#include <Adafruit_FRAM_I2C.h>

Adafruit_FRAM_I2C fram     = Adafruit_FRAM_I2C();

#include "FRAMLog.h"


// BMP180 Sensor

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displayBMP180SensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}
 

// HTU21D-F Humidity Sensor
#include "Adafruit_HTU21DF.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();




#include <stdarg.h>
void p(char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.print(tmp);
}


// no-cost stream operator as described at 
// http://sundial.org/arduino/?page_id=119
template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }

#define PREFIX ""

WebServer webserver(PREFIX, WEB_SERVER_PORT);

// commands are functions that get called by the webserver framework
// they can read any posted data from client, and they output to server

void jsonCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{

  currentObjectStructure parsedObject; 
  initParsedObject(&parsedObject);
  
#ifdef DEBUG 
Serial.println("------------------");
#endif    


  char myBuffer[1024];
  


 
  server.httpSuccess("application/json");
  
  if (type == WebServer::HEAD)
    return;
    
    int myChar;
    int count;
    myChar = server.read();
    count = 0;
 
    
    while (myChar > 0)
    {
     myBuffer[count] = (char) myChar;
     
      myChar = server.read();
      
      count++;

    }
    myBuffer[count] = '\0';

    delay(25);

    messageCount++;
 
    char returnJSON[500] = "";
   
  
   ExecuteCommandAndReturnJSONString(myBuffer, messageCount, md5str, &parsedObject, returnJSON, returnJSON);


#ifdef DEBUG
    Serial.print("returnJSON =");
    Serial.println(returnJSON);
#endif


  
  server << returnJSON;
  
#ifdef DEBUG
    Serial.print("Mem1=");
    Serial.println(freeMemory());
    Serial.println("------------------");
#endif
}


elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs

// setup the RasPiConnect Graph Arrays
SDL_RasPiGraphLibrary windSpeedGraph(10,SDL_MODE_LABELS);
SDL_RasPiGraphLibrary windGustGraph(10,SDL_MODE_LABELS);
SDL_RasPiGraphLibrary windDirectionGraph(10,SDL_MODE_LABELS);

   
void setup()
{
  
  Serial.begin(57600);           // set up Serial library at 9600 bps
  
  Serial.println();
  Serial.println();
  Serial.print(F("WeatherPiArduino "));
  Serial.print(1.1);
  Serial.println(F(" 01/15/2014")); 
  Serial.print(F("Compiled at:"));
  Serial.print (F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  

  //p("ip address = %i.%i.%i.%i\n", ip[0], ip[1], ip[2], ip[3]);
  p("port number = %i\n", WEB_SERVER_PORT);
  //p("mac address = %#X %#X %#X %#X %#X %#X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  
  randomSeed(analogRead(0));

#ifdef ETHERNET
  Ethernet.begin(mac, ip);
#endif


  webserver.begin();


  
  webserver.setDefaultCommand(&jsonCmd);
  webserver.addCommand("arduino", &jsonCmd);
  messageCount=0;
  
  
  unsigned char* hash=MD5::make_hash(PASSWORD);
  //generate the digest (hex encoding) of our hash
  md5str = MD5::make_digest(hash, 16);

  strcpy(bubbleStatus, "ArduinoPiArduino Weather Started\0");
  
  weatherStation.setWindMode(SDL_MODE_SAMPLE, 5.0);
  rainTotal = 0;

  timeElapsed = 0;
  
  // Change to set time and uncomment
  /*
  tmElements_t t_elements; // elements array to date of quitting smoking.

  time_t t;
  
  
  t_elements.Second = 0;
  t_elements.Minute = 13;
  t_elements.Hour = 4;
  t_elements.Wday = 4;
  t_elements.Day = 22;
  t_elements.Month = 1;
  t_elements.Year = 2015 - 1970;
  t = makeTime(t_elements); // Unix timestamp quit date/time
  
  RTC.set(t);
  */
  
  // BMP180
  
  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BM180 ... check your connections */
    Serial.print("Ooops, no BMP180 detected ... Check your wiring or I2C ADDR!");
  }
  
  if (!htu.begin()) {
    Serial.println("Couldn't find HTU21D-F sensor!");
  }
  
  /* Display some basic information on this sensor */
  displayBMP180SensorDetails();
  
  if (fram.begin()) 
  {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    Serial.println("Found I2C FRAM");
    
    initializeFRAMTable(104); // initialize with 104 size
    

    
    
  } 
  else 
  {
    Serial.println("No I2C FRAM found ... check your connections\r\n");

  }
  
  
  // Lightning Detector
  
    //I2C library initialization
  I2c.begin();
  I2c.pullup(true);
  I2c.setSpeed(1); // 1 - 400kHz, 0 - 100kHz
  
  // optional control of power for AS3935 via a PNP transistor
  // very useful for lockup prevention and power saving
  //pinMode(4, OUTPUT);

  // reset all internal register values to defaults
  AS3935.reset();
  // and run calibration
  // if lightning detector can not tune tank circuit to required tolerance,
  // calibration function will return false
  Serial.println("AS3935 Calibration Started");
  if(!AS3935.calibrate())
    Serial.println("AS3935 Tuning out of range, check your wiring, your sensor.");
  Serial.println("AS3935 Calibration Completed");
  // since this is demo code, we just go on minding our own business and ignore the fact that someone divided by zero

  // first let's turn on disturber indication and print some register values from AS3935
  // tell AS3935 we are indoors, for outdoors use setOutdoors() function
  //AS3935.setIndoors();
  AS3935.setOutdoors();
  // turn on indication of distrubers, once you have AS3935 all tuned, you can turn those off with disableDisturbers()
  
  //AS3935.enableDisturbers();  
  AS3935.disableDisturbers();  

  AS3935.setNoiseFloor(0x02);
  printAS3935Registers();
  AS3935IrqTriggered = 0;
  // Using interrupts means you do not have to check for pin being set continiously, chip does that for you and
  // notifies your code
  // demo is written and tested on ChipKit MAX32, irq pin is connected to max32 pin 2, that corresponds to interrupt 1
  // look up what pins can be used as interrupts on your specific board and how pins map to int numbers

  // ChipKit Max32 - irq connected to pin 2
  // attachInterrupt(1,AS3935Irq,RISING);
  // uncomment line below and comment out line above for Arduino Mega 2560, irq still connected to pin 2, same for atmega328p
  
  // Pin 3 is interrupt 1 on mega 2560
  // comment out to use polls
  //attachInterrupt(1,AS3935Irq,RISING);
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();
  //Serial.println(freeMemory());
  
  // update weather
  currentWindSpeed = weatherStation.current_wind_speed()/1.6;


  if (timeElapsed > 5000) 
  {				
    
    Serial.print("time=");
    Serial.println(millis());
    Serial.print("micro time=");
    Serial.println(micros());
    currentWindSpeed = weatherStation.current_wind_speed()/1.6;
    currentWindGust = weatherStation.get_wind_gust()/1.6;
    
    float oldRain = rainTotal;
    rainTotal = rainTotal + weatherStation.get_current_rain_total()*0.03937;
    if (oldRain < rainTotal)
    {
      strcpy(bubbleStatus, "It is Raining\0");
    }
      
     
    timeElapsed = 0;			 // reset the counter to 0 so the counting starts over...
    windSpeedGraph.add_value(currentWindSpeed);
    windGustGraph.add_value(currentWindGust);
    windDirectionGraph.add_value(weatherStation.current_wind_direction());
    
    windSpeedGraph.getRasPiString(windSpeedBuffer, windSpeedBuffer);
    windGustGraph.getRasPiString(windGustBuffer, windGustBuffer);
    windDirectionGraph.getRasPiString(windDirectionBuffer, windDirectionBuffer);
    
    windSpeedMin = windSpeedGraph.returnMinValue();
    windSpeedMax = windSpeedGraph.returnMaxValue();
    windGustMin = windGustGraph.returnMinValue();
    windGustMax = windGustGraph.returnMaxValue();
    windDirectionMin = windDirectionGraph.returnMinValue();
    windDirectionMax = windDirectionGraph.returnMaxValue();
    
    Serial.print("windSpeedMin =");
    Serial.print(windSpeedMin);   
    Serial.print(" windSpeedMax =");
    Serial.println(windSpeedMax);
    
    Serial.print("windSpeedBuffer=");
    Serial.println(windSpeedBuffer);
    
    Serial.print("windGustMin =");
    Serial.print(windGustMin);   
    Serial.print(" windGustMax =");
    Serial.println(windGustMax);
        
    Serial.print("windGustBuffer=");
    Serial.println(windGustBuffer);
    
    Serial.print("windDirectionMin =");
    Serial.print(windDirectionMin);   
    Serial.print(" windDirectionMax =");
    Serial.println(windDirectionMax);
        
    
    Serial.print("windDirectionBuffer=");
    Serial.println(windDirectionBuffer);
    
    Serial.print(" currentWindSpeed=");
    Serial.print(currentWindSpeed);
  
    Serial.print(" \tcurrentWindGust=");
    Serial.print (currentWindGust);
  
    Serial.print(" \tWind Direction=");
    Serial.print(weatherStation.current_wind_direction());
  
   
    Serial.print(" \t\tCumulative Rain = ");
    Serial.print(rainTotal);
    Serial.print("\t\tmemory free=");
    Serial.println(freeMemory());
  
    tmElements_t tm;
    Serial.println("---------------");  
    Serial.println("DS3231 Clock");
    Serial.println("---------------");    
    RTC.read(tm);
  
    printDigits(tm.Hour);
    Serial.print(":");
    printDigits(tm.Minute);
    Serial.print(":");
    printDigits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    printDigits(tm.Day);
    Serial.print('/');
    printDigits(tm.Month);
    Serial.print('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
    float DS3231_Temp;
    DS3231_Temp =RTC.get_treg();
    Serial.print("Temperature= ");
    Serial.println(DS3231_Temp);
    
    Serial.println("---------------");  
    Serial.println("AT24C32 EEPROM");
    Serial.println("---------------");
    Serial.println("Write 5 addresses with random numbers and read them back");
   
   unsigned int i;
   byte  readValue;
   byte  writeValue;
   for (i=0; i < 5; i++)
   {
     writeValue = random(0,255);
     i2c_eeprom_write_byte( 0x56, i, writeValue );
     Serial.print("Address: ");
     Serial.print(i);
     Serial.print(" Write:");
     Serial.println(writeValue);
     
     
   }
   // Now read
  for (i=0; i < 5; i++)
   {
     readValue = i2c_eeprom_read_byte( 0x56, i );
     Serial.print("Address: ");
     Serial.print(i);
     Serial.print(" Read Value:");
     Serial.println(readValue);
     
     
   }
     

   Serial.println("---------------");
   Serial.println("BMP180");
   Serial.println("---------------");
   /* Get a new sensor event */ 
   sensors_event_t event;
   bmp.getEvent(&event);
   float BMP180_Temp;
   float BMP180_Pressure;
  /* Display the results (barometric pressure is measure in hPa) */
   if (event.pressure)
    {
      /* Display atmospheric pressue in hPa */
      Serial.print("Pressure:    ");
      Serial.print(event.pressure);
      Serial.println(" hPa");
      
      BMP180_Pressure = event.pressure;
      
      /* Calculating altitude with reasonable accuracy requires pressure    *
       * sea level pressure for your position at the moment the data is     *
       * converted, as well as the ambient temperature in degress           *
       * celcius.  If you don't have these values, a 'generic' value of     *
       * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA   *
       * in sensors.h), but this isn't ideal and will give variable         *
       * results from one day to the next.                                  *
       *                                                                    *
       * You can usually find the current SLP value by looking at weather   *
       * websites or from environmental information centers near any major  *
       * airport.                                                           *
       *                                                                    *
       * For example, for Paris, France you can check the current mean      *
       * pressure and sea level at: http://bit.ly/16Au8ol                   */
     
      /* First we get the current temperature from the BMP085 */
      float temperature;
      bmp.getTemperature(&temperature);
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");
      

      BMP180_Temp =temperature;


      /* Then convert the atmospheric pressure, and SLP to altitude         */
      /* Update this next line with the current SLP for better results      */
      float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
      Serial.print("Altitude:    "); 
      Serial.print(bmp.pressureToAltitude(seaLevelPressure,
                                        event.pressure)); 
      Serial.println(" m");

    }
    

    Serial.println("---------------");
    Serial.println("HTU21D-F");
    Serial.println("---------------");
    float HTU_Temp;
    HTU_Temp =htu.readTemperature();
    Serial.print("Temperature: "); Serial.print(HTU_Temp);Serial.println(" C");
    Serial.print("Humidity: "); Serial.print(htu.readHumidity()); Serial.println(" %");  
    Serial.println("---------------");
   
    
    Serial.println("---------------");
    Serial.println("Lightning Detection");
    Serial.println("---------------");    
      
    // reset the flag
    AS3935IrqTriggered = 0;
    // first step is to find out what caused interrupt
    int strokeDistance = 0.0;
    int irqSource = 0;
     
    // comment out the next two lines if you want to just poll instead of interrupt   
    //       if(AS3935IrqTriggered)
    //{
        irqSource = AS3935.interruptSource();
        Serial.print("irqSource: ");
        Serial.println(irqSource, HEX);
        


    // comment out the next two lines if you wish to use interrupts    
    if(irqSource > 0)
    {
  
      // returned value is bitmap field, bit 0 - noise level too high, bit 2 - disturber detected, and finally bit 3 - lightning!
      if (irqSource & 0b0001)
          Serial.println("INT_NH Interrupt: Noise level too high, try adjusting noise floor");
      if (irqSource & 0b0100)
          Serial.println("INT_D Interrupt: Disturber detected");
      if (irqSource & 0b1000)
      {
        // need to find how far that lightning stroke, function returns approximate distance in kilometers,
        // where value 1 represents storm in detector's near victinity, and 63 - very distant, out of range stroke
        // everything in between is just distance in kilometers
        strokeDistance = AS3935.lightningDistanceKm();
        
        Serial.print("INT_L Interrupt: Lightning Detected.  Stroke Distance:");
        Serial.print(strokeDistance);
        Serial.println(" km");
        if (strokeDistance == 1)
          Serial.println("Storm overhead");
        if (strokeDistance == 63)
          Serial.println("Out of range lightning detected.");
   
      }
    }
    Serial.println("---------------");
   
    Serial.println("---------------");
    Serial.println("FRAM Log");
    Serial.println("---------------");   
     // now write the record to FRAM
    
    
    char returnString[100];
    returnString[0] = '\0';
           


    RTC.read(tm);


    char floatString[15];

    char timeNow[20];
    timeNow[0] = '\0';
    buildTimeString(timeNow, timeNow, tm);
    

          
    sprintf(returnString, "%s,", timeNow);
           
    floatString[0] = '\0';
    dtostrf(currentWindSpeed,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(currentWindGust,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 
    floatString[0] = '\0';
    dtostrf(weatherStation.current_wind_direction(),6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");

    floatString[0] = '\0';
    dtostrf(weatherStation.current_wind_direction_voltage(),6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");

    floatString[0] = '\0';
    dtostrf(rainTotal,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
 


    floatString[0] = '\0';
    dtostrf(DS3231_Temp,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
    
    floatString[0] = '\0';
    dtostrf(HTU_Temp,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ",");
    
    floatString[0] = '\0';
    dtostrf(BMP180_Temp,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ", ");
    
    floatString[0] = '\0';
    dtostrf(BMP180_Pressure,6,2,floatString);
    strcat(returnString, floatString);
    strcat(returnString, ", ");

    floatString[0] = '\0';
    itoa(irqSource,floatString,10);
    strcat(returnString, floatString);
    strcat(returnString, ", ");

    floatString[0] = '\0';
    dtostrf(strokeDistance,6,2,floatString);
    strcat(returnString, floatString);




    
   

    
    
    writeFramEntry(0,  returnString);
    
    Serial.println(returnString);

    // displayFram();
    
  
  }

  // if you wanted to do other work based on a connecton, it would go here
}
