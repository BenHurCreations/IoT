/*
  SmartHomie
SmartHomie is a four widgets based system that enables the user control remotely different aspects of his life.

- The first abilty enables working more productively in "work mode", which makes sure the user keeps his smartphone face down.
  This way the incoming notifications do not distract him.
  Whenever the smartphone faces up- lights start to blink & nudge tone starts to play-
  untill the user puts his phone with the face down once again. 
  We use the accelerometer widget to measure the force applied on the z axis of the smartphone, whenever it's positive- 
  the nudges are being activated.

- The second ability is to monitor the temprature at home using the cpx temprature sensor.
  We used the gauge widget to represent this data visually.

- The third ability is to turn on / off the TV using the cpx infra-red transmitter, using the button widget.

- The fourth ability allows us to control the light in our home (the cpx neo pixels) using the vertical slider widget.
  The higher the value we get, the brighter the light is.

  Welcome to your new SmartHomie!

The circuit:
  * No ports are used for the code design

  Video link: https://youtu.be/NcAQs3vaNF4
  Created By:
  Yoav_Ben_Hur 
  Gal_Kimron   
*/

#if !defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
    #error "Infrared support is only for the Circuit Playground Express, it doesn't work with the Classic version"
#endif

#define BLYNK_PRINT SerialUSB

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>

char auth[] = "mjMPOxtHu0-ZpwSZVRT_m0igEFv7yYWI";
char ssid[] = "deborah";
char pass[] = "0545294093";

#define EspSerial Serial1
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

float tempC;

// Init vars for the TV controller
#define IR_protocol 1
#define IR_value 0xFEA05F
#define IR_bits 32


void setup() {
  CircuitPlayground.begin();
  SerialUSB.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
}


void loop() {
  Blynk.run();
  
  // Get the room tempratues and send it to blink gauge widget feature
  tempC = CircuitPlayground.temperature();
  Serial.println(tempC);
  Blynk.virtualWrite(V1, tempC);
}


// Work mode feature
BLYNK_WRITE(V0) {
  //acceleration force applied to axis x
  int x = param[0].asFloat(); 
  //acceleration force applied to axis y
  int y = param[1].asFloat();
  //acceleration force applied to axis y
  int z = param[2].asFloat();
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("y: ");
  Serial.print(y);
  Serial.print("z: ");
  Serial.println(z);
  if(z > 0){
       Serial.println("Flip your phone to work mode");
       CircuitPlayground.playTone(500, 100);
       CircuitPlayground.clearPixels();
       delay(500);
       CircuitPlayground.setPixelColor(0,  0,   0, 255);
       CircuitPlayground.setPixelColor(1,  0, 128, 128);
       CircuitPlayground.setPixelColor(2,  0,   0, 255);
       CircuitPlayground.setPixelColor(3,  0, 128, 128);
       CircuitPlayground.setPixelColor(4,  0,   0, 255);
       CircuitPlayground.setPixelColor(5,  0, 128, 128);
       CircuitPlayground.setPixelColor(6,  0,   0, 255);
       CircuitPlayground.setPixelColor(7,  0, 128, 128);
       CircuitPlayground.setPixelColor(8,  0,   0, 255);
       CircuitPlayground.setPixelColor(9,  0, 128, 128);
       delay(500);
       CircuitPlayground.clearPixels();
    }
    else{
        CircuitPlayground.clearPixels();
    }
}


// Turn on / off the TV feature
BLYNK_WRITE(V2) {
    int onOff = param.asInt();
    if(onOff){
        for(int i = 0; i <= 9; i++){
          CircuitPlayground.irSend.send(IR_protocol, IR_value, IR_bits);
          delay(30);
        }
    }
}


// Turn on + change the lights brightness feature
BLYNK_WRITE(V3) {
    int slider = param.asInt();
    CircuitPlayground.clearPixels();
    for(int i = 0; i <= 9; i++){
        CircuitPlayground.setPixelColor(i, slider, slider/2, slider/4);   
    }
}
