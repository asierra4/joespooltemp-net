//-------------------------------------------------------------------
//
// Joe's Pool Temp
// Ada - May 2018
//
//------------------------------------------------------------------

// Libraries for the DS18B20 Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// Config (incl Libraries) for AdafruitIO
#include "config.h"


// ************************************************
// Pin definitions
// ************************************************

// One-Wire Temperature Sensor
// (Use GPIO pins for power/ground to simplify the wiring?)
#define ONE_WIRE_BUS 12
//#define ONE_WIRE_PWR 3
//#define ONE_WIRE_GND 4

// ************************************************
// Sensor Variables and constants
// ************************************************
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
 
// arrays to hold device address
DeviceAddress tempSensor;

// ************************************************
// Cloud (AdafruitIO) Data stream setup
// ************************************************
AdafruitIO_Feed *temperature1 = io.feed("pool-temp");

// Debugging mode (to serial terminal)
#define LOG_TO_SERIAL 0
#define TIME_LOG_INTERVAL 10
#define MEASURE_INTERVAL_SEC 60

void setup() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  
  #if LOG_TO_SERIAL
    // start the serial connection
    Serial.begin(115200);// wait for serial monitor to open
    while(! Serial);
  #endif
    
  // connect to io.adafruit.com
  #if LOG_TO_SERIAL
    Serial.println("Connecting to Adafruit IO");
  #endif
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    #if LOG_TO_SERIAL
      Serial.print(".");
    #endif
    delay(500);
  }
  
  #if LOG_TO_SERIAL
    // we are connected
    Serial.println();
    Serial.println(io.statusText());
  #endif

  //sensors initialization using Dallas Temperature library
  sensors.begin();
  
  #if LOG_TO_SERIAL
    Serial.println("Enabling Low Power Mode...");
  #endif    
  WiFi.lowPowerMode();
  
  #if LOG_TO_SERIAL
  Serial.println("Setup Complete.");
  #endif}

void loop() {

  io.run(); //adafruit-io keeps the client connected to io.adafruit.com, and processes any incoming data.
  
  float tempF0;
  #if LOG_TO_SERIAL
    Serial.println("Requesting temperatures...");
  #endif
  sensors.requestTemperatures();
  tempF0=sensors.getTempFByIndex(0);
  #if LOG_TO_SERIAL
    Serial.print("temperature: ");
    Serial.print(tempF0);
    Serial.println("F");
    Serial.println("Sending to adafruit.io...");
  #endif
  temperature1->save(tempF0);

  #if LOG_TO_SERIAL
    Serial.print("Counting to ");
    Serial.println(MEASURE_INTERVAL_SEC);
  #endif
  
  //wait until next measurement is needed
  for (int i=0; i<MEASURE_INTERVAL_SEC;i=i+TIME_LOG_INTERVAL){
    #if LOG_TO_SERIAL
      Serial.print(i);      
      Serial.print(" ");
    #endif
    delay(TIME_LOG_INTERVAL*1000);
  }
  #if LOG_TO_SERIAL
      Serial.println("");
  #endif
}


