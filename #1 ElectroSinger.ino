/*
  Electro Singer
Electro Singer is our first device built with adafruit circuit playground!
It uses ports A1-A5 to detect tapping on 5 different veggies-
avocado, guyava, clemantine, banana & potato!

Each tap changes the capacitivity values of the sensors and triggers a relevant sound.
We used the light sensor to determine the music tone.
With regular light we have medium tone. With extra light we have higher tone, 
and with little light we have low tone.
The second output is led light. Any tap the code produces a different color.

We chose to play "Little Yonatan", the beloved children's song.
Have fun watching!

  The circuit:
  * Ports A1-A5 are attached to the drum-fruits by alligators
  * The change in capacitivity in A1-A5 capacitivity sensors triggers the relevant sound
  * The amount of light determines the music tone

  Video link: https://youtu.be/rAO612zG3VQ
  Created By:
  Yoav_Ben_Hur 
  Hadar_Okashi 
*/ 


#include <Adafruit_CircuitPlayground.h>
#include <stdlib.h>


// High-Mid-Low tones, do-re-mi-fa-sol
float notes[3][5] = {
    {261.626, 294.33, 327.03, 348.83, 392.44},
    {523.25, 587.33, 659.25, 698.46, 783.99},
    {1046.50, 1174.66, 1318.51, 1396.91, 1567.98}
};

// Light sensor value & boundaries
float lightStrength, lightLow, lightHigh, toneChoose;

// Capacitive sensors ports
byte capSense[] = {
    A1, A2, A3, A4, A5
};

// Capacitive sensors values
float capValues[5];

// Track change variable
float capDelta;

// Delta value, chosen after testing all the vegies
float DELTA = 100;


// Initialization
void setup() {
    Serial.begin(9600);
    CircuitPlayground.begin();

    // Initiate capacitive sensors values
    for(int i = 0; i < sizeof(capSense); i++){
        capValues[i] = CircuitPlayground.readCap(capSense[i]);
    }

    //Initiate light sensor value
    lightStrength = CircuitPlayground.lightSensor();
    lightLow = lightStrength - 15;
    lightHigh = lightStrength + 15;
}


// Electro Drums 
void loop() {

    // Determine the sound tone
    setToneByLight();
  
    for(int i = 0; i < sizeof(capSense); i++){
        // Get capacitivity values
        float cap = CircuitPlayground.readCap(capSense[i]);
        capDelta = cap - capValues[i];
        capValues[i] = cap;

        // If the drummer hit the fruit- light the leds and play the relevant sound
        if(capDelta >= DELTA){
            CircuitPlayground.playTone(notes[(int)toneChoose][i], 100);
            lightBeautifulLeds();
        }

        // Testing touch delta values to determine DELTA
        Serial.print("A");
        Serial.print(i+1);
        Serial.print("= ");
        Serial.print(capDelta);
        Serial.print("   ");
    }
    Serial.print("\n");

    delay(5);
}


// Set music tone by light strength
void setToneByLight() {
    lightStrength = CircuitPlayground.lightSensor();
    if(lightStrength <= lightLow) { toneChoose = 0; }
    else if(lightStrength >= lightHigh) { toneChoose = 2; }
    else { toneChoose = 1; }
    
    // Debugging for light sensor
    Serial.print(lightStrength);
    Serial.print(" ");
    Serial.print(toneChoose);
    Serial.print("\n");
}


// Lighting the leds after each touch
void lightBeautifulLeds(){
    int val1 = rand()%255;
    int val2 = rand()%255;
    int val3 = rand()%255;
    for(int i = 0; i <= 9; i++){
        CircuitPlayground.setPixelColor(i, val1, val2, val3);
    }
}
