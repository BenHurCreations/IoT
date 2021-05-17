/*
  MiDay
MiDay is a light clock whick contatins different features for the user.
It uses a built-in clock function to represent the time using led lights.
Each color represents a clock hand, blue shade represents the minutes while yellow shade represents the hours.
Darker light is activated in PM hours while brighter light is activated during AM hours.

The user can set his alarm clock using the infra-red remote controller.
He chooses the amount of time he wants to sleep and the waking-up time is caclulated accordingly.
After he chooses the time, sleep mode is activated.

During the night, we count how many times the user wakes up using capacitivity sensors and a touch surface on the floor.
Each tap changes the capacitivity values of the sensors and triggers a relevant action.

15 minutes before wake-up time sunrise leds are activated. The colors change from shades of red to orange and finally to yellow.
The alarm clock is then set and the music begins. Simultaneously the wake up leds (yellow) continue to light.
The user now needs to activate a certain password using the joystick. Once he succeeds, the led lights light colorfuly and the alarm clock is stopped.
The clock contnues to work, and the user is asked if he slept good or not. He answers using the rught & left buttons.
This data + the time it took to the user to wake up from bed and finish the joystick challenge is stored for future analysis.

  The circuit:
  * Ports A1-A2 are attached to extra leds.
  * Port A3 is connected to the touch surface.
  * The change in capacitivity in A3 capacitivity sensors helps counting how many times the user got out from his bed during the night
  * Ports A0, A4, A5, 3.3V, GND are connected to the joystick. 
  * 3.3v & GND for power, A0 for the x axis of the joystick, A4 for the y axis & A5 for the Switch Pin of the joystick.

  Video link: https://drive.google.com/file/d/1-KwOVF5N285Yl6_ldMn2fKg64BsDLtNk/view?usp=sharing
  Created By:
  Gal_Kimron   
  Yoav_Ben_Hur 
  Hadar_Okashi 
*/


/////////////////////////////////Dependencies declaration + variables initiaing///////////////////////////

#include <Adafruit_CircuitPlayground.h>
#include <stdlib.h> //for representation time

#if !defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
    #error "Infrared support is only for the Circuit Playground Express, it doesn't work with the Classic version"
#endif


// Init vars for remote (set alarm) function
uint8_t IR_protocol;
uint32_t IR_value;
uint16_t IR_bits;
int num_hours=0 ,num_10th_min=0;


// Init vars for main clock function
int second=0, minutes=26, hours=1; // init vars for the main clock
bool timeFlag = true; //if timeFlag == true its AM now,if timeFlag = false -> its PM now


// Init vars for calculate timeToWakeUp
int timeToWakeUp_minute=0, timeToWakeUp_hour=0;
bool isSleeping = false;


// Init vars for Joystick
int Xpin=A0;
int Ypin=A4;
int Spin=A5;
int Xval;
int Yval;
int Sval;
int dt=200;
int passwordCounter = 0;
bool ifSucceedPassword = false;


// Init Music Variables
bool stopMusic = false;


// Init capacitive sensor variables
float capStrength, capDelta;
float lastCapStrength = 10000;
boolean is_in_bed = true;
float times_woke_up = 0;


// Init color Variables
float value = 0;

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Dependencies declaration + variables initiaing^^^^^^^^^^^^^^^^^^^^^^^^^^^


void setup()
{
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Send an IR signal and I will record it.");
    CircuitPlayground.begin();
  
    //activate IR-Receiver
    CircuitPlayground.irReceiver.enableIRIn(); // for remote(set alarm) function. Start the receiver
    IR_protocol=0; //for remote(set alarm) function Indicates we've not received a code yet
  
    //init JoyStick Pins
    pinMode(Xpin,INPUT);
    pinMode(Ypin,INPUT);
    pinMode(Spin,INPUT);
    digitalWrite(Spin,HIGH);
  
    //init vars for lights representation of the clock
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);  
}


void loop()
{
    /////////////////////////////////////For main clock function//////////////////////////////////////////
    Serial.print("Time : ");
  
    //check AM/PM
    if(timeFlag)
    {
        Serial.print(" (AM)");
    }
    else
    {
        Serial.print("  (PM)");
    }
    
    //add one second every loop
    second++;
  
    //add one minute every 60 minutes
    if(second>=60)
    {
        second=0;
        minutes++;
    }
  
    //add one hour every 60 minutes 
    if(minutes>=60)
    {
        second=0;
        minutes=0;
        hours++;
    }
  
    //verify if hours more then 12
    if(hours>=13)
    {
        second=0;
        minutes=0;
        hours=1;
        timeFlag = !timeFlag;
    }
  
    //print the time
    Serial.print("hours is: ");
    Serial.print(hours);
    //Serial.print(" minutes: ");
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(second);
    Serial.print("time to wake up------------->");
    Serial.println(timeToWakeUp_minute);
  
    //the main delay of the clock = 1000 mis - sum of all the delays in the program  
    delay(100);
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^For main clock function^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


    // Clock representation activation
    activate_light_clock(hours, minutes, timeFlag);


    ///////////////////////////////////////For setting the alarm clock/////////////////////////////////////////
   
    //init IR for set-alarm
    if(CircuitPlayground.irReceiver.getResults()) {
        
        //attempt to decode it
        if(CircuitPlayground.irDecoder.decode()) {
            Serial.println("IR decoded");
            
            //Print the results.  Change parameter to "true" for verbose output.
            CircuitPlayground.irDecoder.dumpResults(false);
            Serial.println("Saving results. Press left button to retransmit.");
            IR_protocol=CircuitPlayground.irDecoder.protocolNum;
            IR_value= CircuitPlayground.irDecoder.value;
            IR_bits= CircuitPlayground.irDecoder.bits;
        
            //if received IR "off"('exit' button) button then turn-off the alarm system.
            //Also gives feedback to the user with red neo-pixel blink to the user
            if(IR_protocol == 1 && IR_value == 0x4FBCA35 && IR_bits == 32){
                isSleeping = false;
                Serial.print("isSleeping mode: ");
                Serial.println(isSleeping);
                CircuitPlayground.setPixelColor(8, 0xFF0000);
                delay(20);
                CircuitPlayground.clearPixels();
                num_hours=0;
                num_10th_min=0;
            }
            
            //if received IR "on"('Sleep' button) button then turn-on the sleeping mode.
            //Also gives feedback to the user with green neo-pixel blink to the user
            if(IR_protocol == 1 && IR_value == 0x4FBB847 && IR_bits == 32){
                isSleeping = true;
                Serial.print("isSleeping mode: ");
                Serial.println(isSleeping);
                CircuitPlayground.setPixelColor(8, 0x00ff00);
                delay(20);
                CircuitPlayground.clearPixels();

                //init vars for Joystick Synchronization
                passwordCounter = 0;
                ifSucceedPassword = false;
                isSleeping = true;
                stopMusic = false;
            }
            
            //if received IR "up an hour" (Vol up button).
            if(IR_protocol == 1 && IR_value == 0x4FB40BF && IR_bits == 32){
                num_hours += 1;
                //blink neo pixel for indication for user that the clock got the push on the button 
                CircuitPlayground.setPixelColor(9, 0x0000FF);
                delay(100);
                CircuitPlayground.clearPixels();
                Serial.print("num_hours is: ");
                Serial.println( num_hours);     
            }
        
            //if received IR "down an hour" (Vol down button).
            if(IR_protocol == 1 && IR_value == 0x4FB906F && IR_bits == 32){
                num_hours -= 1;
                //blink neo pixel for indication for user that the clock got the push on the button 
                CircuitPlayground.setPixelColor(9, 0x0000FF);
                delay(100);
                CircuitPlayground.clearPixels();
                if(num_hours <0){
                    num_hours =0;
                }
                Serial.print("num_hours is: ");
                Serial.println( num_hours);     
            }
        
            //if received IR "up an 10th min" (CH up button).
            if(IR_protocol == 1 && IR_value == 0x4FBC03F && IR_bits == 32){
                num_10th_min += 1;
                //blink neo pixel for indication for user that the clock got the push on the button 
                CircuitPlayground.setPixelColor(9, 0x0000FF);
                delay(100);
                CircuitPlayground.clearPixels();
                Serial.print("num_10th_min: ");
                Serial.println(num_10th_min);     
            }
        
            //if received IR "down an 10th min" (CH down button).
            if(IR_protocol == 1 && IR_value == 0x4FB827D && IR_bits == 32){
                num_10th_min -= 1;
                //blink neo pixel for indication for user that the clock got the push on the button 
                CircuitPlayground.setPixelColor(9, 0x0000FF);
                delay(100);
                CircuitPlayground.clearPixels();
                if(num_10th_min <0){
                    num_10th_min =0;
                }
                Serial.print("num_10th_min: ");
                Serial.println(num_10th_min);     
            }
        }
        CircuitPlayground.irReceiver.enableIRIn();  //Restart receiver
    }
    
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^For setting the alarm clock^^^^^^^^^^^^^^^^^^^^^^^^^^^^


    ////////////////////////////////////Calculate timeToWakeUp//////////////////////////////////////////
    
    timeToWakeUp_minute = num_10th_min*10 + minutes;
    timeToWakeUp_hour = num_hours + hours;
    
    //if minutes over 60, add 1 hour
    if(timeToWakeUp_minute > 60){
        timeToWakeUp_minute -= 60;
        timeToWakeUp_hour += 1;  
    }
    
    if(timeToWakeUp_hour > 12){
        timeToWakeUp_hour -= 12;
    }

    //print the time that user want to wake-up
    Serial.print("Time to wake up, hours: ");
    Serial.println(timeToWakeUp_hour);
    Serial.print("Time to wake up, minutes: ");
    Serial.println(timeToWakeUp_minute);
    
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Calculate timeToWakeUp^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


    //////////////////////////////////////////////Joystick///////////////////////////////////////////////
    
    //read Joystick-pins
    Xval=analogRead(Xpin);
    Yval=analogRead(Ypin);
    Sval=digitalRead(Spin);
    delay(dt);

    //print Joystick value
    Serial.print("X Value = ");
    Serial.println(Xval);
    Serial.print(" Y Value = ");
    Serial.println(Yval);
    Serial.print(" Switch State is ");
    Serial.println(Sval);
  
    //check if the user is sleeping to read Joystick
    if(isSleeping){

        //check if the user made the first move in the sequance
        if(Xval > 650){
            passwordCounter = 1;
        }
  
        //check if the user made the second move in the sequance
        if (passwordCounter == 1 && Yval > 650){
            passwordCounter = 2; 
        }
  
        //check if the user made the third move in the sequance
        if(passwordCounter ==2 && Xval < 300){
            passwordCounter = 3;
        }
  
        //check if the user made the fourth move in the sequance
        if(passwordCounter ==3 && Yval < 300){
            passwordCounter =4;
        }
  
        //print for convenience for the grader
        Serial.print("passwordCounter is----------------------------------------->: ");
        Serial.println(passwordCounter); 
  
        //check if user succeed password and chage the ifSucceedPassWord flag to true
        if(passwordCounter == 4){
            ifSucceedPassword = true;
            passwordCounter = 0;
        }

        //turn on neo-pixels if user succeed password
        if(ifSucceedPassword){
            CircuitPlayground.setPixelColor(0, 255,   0,   0);
            CircuitPlayground.setPixelColor(1, 128, 128,   0);
            CircuitPlayground.setPixelColor(2,   0, 255,   0);
            CircuitPlayground.setPixelColor(3,   0, 128, 128);
            CircuitPlayground.setPixelColor(4,   0,   0, 255);
            CircuitPlayground.setPixelColor(5, 0xFF0000);
            CircuitPlayground.setPixelColor(6, 0x808000);
            CircuitPlayground.setPixelColor(7, 0x00FF00);
            CircuitPlayground.setPixelColor(8, 0x008080);
            CircuitPlayground.setPixelColor(9, 0x0000FF);
            delay(100);

            //clear Neo-Pixels
            CircuitPlayground.clearPixels();
  
            //init isSleeping for be ready for next time
            isSleeping = false;
        }
    }
  
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Joystick^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


    ///////////////////////////////////////////Wake-up Music///////////////////////////////////////////
    if(ifSucceedPassword){
        stopMusic = true;
        ifSucceedPassword = false;
    }

    // Before activating the music&light wake-up -> check if it is the right time, and if user set the alarm-clock
    // and check if the user didnt succeed the Joystick mission
    if(isSleeping && hours == timeToWakeUp_hour  && minutes >= timeToWakeUp_minute && minutes <= timeToWakeUp_minute + 5 && !stopMusic){
       for(int i = 0; i < 1760; i += 3) {
           CircuitPlayground.playTone(i, 5);
       }
    }  


    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Wake-up Music^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


    ////////////////////////////////////////////Counting number of out-of-bed times//////////////////////

    // Check how many times the user got out from the bed using capacitive sensors
    if(isSleeping){
        capStrength = CircuitPlayground.readCap(A3);
        capDelta = capStrength - lastCapStrength;
        if(capDelta > 60){
           if(is_in_bed){
              is_in_bed = false;
           }
           else{
              is_in_bed = true;
              times_woke_up += 1;
           }
           lastCapStrength = capStrength;
        }
        if(is_in_bed){
            Serial.println("User is in bed");
        }
        else{
            Serial.println("User is up");
        }
        Serial.print("User woke up ");
        Serial.print(times_woke_up);
        Serial.println(" times");
    }


    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Counting number of out-of-bed times^^^^^^^^^^^^^^^^^^^^^^


    // Activating sunrise + wake up leds conditions
    if(isSleeping){
        // Sunrise led activation (15 mins before wake up)
        if((hours == timeToWakeUp_hour && minutes == timeToWakeUp_minute - 15) || (hours == timeToWakeUp_hour - 1 && minutes == timeToWakeUp_minute + 45)){
            activate_sunrise_led();  
        }
  
        // Wake up led activation
        if(hours == timeToWakeUp_hour && minutes == timeToWakeUp_minute){
            activate_wake_up_led(); 
        }

    }
}


// For remote (set alarm) function
// Calculate the time until waking-up.
// Get input from the controller.
int calculate_sleep_time(int num_hours ,int num_10th_min){
    return num_hours*60+num_10th_min*2;
}


///////////////////////////////////////////Light representaion-clock function///////////////////////////////
// Lights representation of the clock
void activate_light_clock(float hour, float min, bool day){
    
    turn_off_all();
    float i,j,k,l,m,n;
    
    if(day){
        i = 255;
        j = 255;
        k = 0;
        l = 100;
        m = 255;
        n = 100;
    }
    
    else{
        i = 255;
        j = 50;
        k = 0;
        l = 0;
        m = 120;
        n = 255;
    }
    
    // Hour light
    if(hour == 1) CircuitPlayground.setPixelColor(9, i, j, k);
    else if(hour == 2) CircuitPlayground.setPixelColor(8, i, j, k);
    else if(hour == 3) CircuitPlayground.setPixelColor(7, i, j, k);
    else if(hour == 4) CircuitPlayground.setPixelColor(6, i, j, k);
    else if(hour == 5) CircuitPlayground.setPixelColor(5, i, j, k);
    else if(hour == 6) digitalWrite(A1, HIGH);
    else if(hour == 7) CircuitPlayground.setPixelColor(4, i, j, k);
    else if(hour == 8) CircuitPlayground.setPixelColor(3, i, j, k);
    else if(hour == 9) CircuitPlayground.setPixelColor(2, i, j, k);
    else if(hour == 10) CircuitPlayground.setPixelColor(1, i, j, k);
    else if(hour == 11) CircuitPlayground.setPixelColor(0, i, j, k);
    else if(hour == 12) digitalWrite(A2, HIGH);
  
    // Minute light
    if(min >= 3 && min <= 7) CircuitPlayground.setPixelColor(9, l, m, n);
    else if(min >= 8 && min <= 12) CircuitPlayground.setPixelColor(8, l, m, n);
    else if(min >= 13 && min <= 17) CircuitPlayground.setPixelColor(7, l, m, n);
    else if(min >= 18 && min <= 22) CircuitPlayground.setPixelColor(6, l, m, n);
    else if(min >= 23 && min <= 27) CircuitPlayground.setPixelColor(5, l, m, n);
    else if(min >= 28 && min <= 32) digitalWrite(A1, HIGH);
    else if(min >= 33 && min <= 37) CircuitPlayground.setPixelColor(4, l, m, n);
    else if(min >= 38 && min <= 42) CircuitPlayground.setPixelColor(3, l, m, n);
    else if(min >= 43 && min <= 47) CircuitPlayground.setPixelColor(2, l, m, n);
    else if(min >= 48 && min <= 52) CircuitPlayground.setPixelColor(1, l, m, n);
    else if(min >= 53 && min <= 57) CircuitPlayground.setPixelColor(0, l, m, n);
    else if(min <= 3 || min >= 58) digitalWrite(A2, HIGH);
}


// Turning off all the leds
void turn_off_all(){
    for(int i = 0; i <= 9; i++){
        CircuitPlayground.setPixelColor(i, 0, 0, 0);
    }
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Light representaion-clock function^^^^^^^^^^^^^^^^^^^^^^^^^^^^


//////////////////////////////////////////////////Activate Sunrise + wakeup Led/////////////////////////////

// Set sunrise leds for 255*3.5 seconds (approx. 15 mins)
void activate_sunrise_led(){
    for(int j = 0; j < 255; j++){
        for(int i = 0; i <= 9; i++){
            CircuitPlayground.setPixelColor(i, 255, value, 0);
            delay(250);
        }
        value += 4;
        delay(10);  
    }
}


// Sets the wake up led to constant bright light
void activate_wake_up_led(){
    for(int i = 0; i <= 9; i++){
        CircuitPlayground.setPixelColor(i, 255, 255, 0);
    }
    delay(10000);
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^Activate Sunrise + wakeup Led^^^^^^^^^^^^^^^^^^^^^
