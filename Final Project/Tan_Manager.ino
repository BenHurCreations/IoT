/*

Final Project!


The circuit:
  * No ports are used for the code design

Address to send the number of firends you passed by-
http://188.166.206.43/mjMPOxtHu0-ZpwSZVRT_m0igEFv7yYWI/update/V5?value=status

  Video link: 
  Created By:
  Yoav_Ben_Hur
  Gal_Kimron  
*/
 
#define BLYNK_PRINT SerialUSB

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>

char auth[] = "mjMPOxtHu0-ZpwSZVRT_m0igEFv7yYWI";
char ssid[] = "Deborah";
char pass[] = "0545294093";

#define EspSerial Serial1
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

// Init clock variables
int second=0, minutes=0, hours=7;

// Sleep mode variable
int isHome = 1;

// #1.1 Proximity sensors init
#include <Proximity.h>
#define SUNGLASSES_THRESHOLD 80
#define DOOR_THRESHOLD 80
#define HAND_THRESHOLD 85
Proximity sunglassesProximity;
Proximity doorProximity;
Proximity prox;

#if !defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS)
#error "Infrared support is only for the Circuit Playground Express, it doesn't work with other boards."
#endif

// #2 Init tan index parameters  
#define TAN_INDEX_LEVEL 15
int sumTanChange = 0;
int tanIndex = 1;

// #2 Init Color parameters  
 uint8_t currentColor[3];
 uint8_t prevColor[3];
 uint8_t basicColor[3];
 bool triggerToGetBaseColor = false;
 bool firstRun = true;

// #3
String recommendTanning[] = {"Go out to the sun Mr. Ashkenazi!", "You're more yoghurt than sexy", 
                              "Take your frisbi and get some sun!", "You're very close to perfect tan, almost there!",
                              "Beautiful tan lines, go get your D vitamin"};
String recommendation;
String weatherStatus;


void setup() {
    // Blynk + wifi connection init
    CircuitPlayground.begin();
    SerialUSB.begin(9600);

    // Wifi connection
    EspSerial.begin(ESP8266_BAUD);
    delay(10);
    Blynk.begin(auth, wifi, ssid, pass);
    
    // Proximity thresholds init
    sunglassesProximity.begin(SUNGLASSES_THRESHOLD);
    doorProximity.begin(DOOR_THRESHOLD);
    prox.begin(HAND_THRESHOLD);
    
    // Reset blynk skin color leds
    Blynk.virtualWrite(V2,0);
    Blynk.virtualWrite(V3,0);
    Blynk.virtualWrite(V4,0);
}


void loop() {
    Blynk.run();
    basicClock();

    // Activate only when user is at home (saving battery life)
    if(isHome){
        // Is the sunglasses on stand
        Serial.println("-----------       Sunglasses      -----------");
        Serial.println(sunglassesProximity.lastDist()); // For testing treshold
        if(sunGlassOnStand()){
            Serial.println("On stand");
        }
        else{
            Serial.println("Not on stand");
        }
        
        // #1 When you forget to take your sunglasses notification
        if(sunGlassOnStand() && doorIsOpen() && sunnyDay()){
            takeSunglassesNotification();
        }
        
        //********************************               #2 Color Scanner                  ******************************//
        // Take base color
        if(CircuitPlayground.rightButton()){
            Serial.println("-----------       Right Button      -----------");
            Serial.print("Rightbutton: ");
            Serial.println(CircuitPlayground.rightButton());
            buttonPressed();
            triggerToGetBaseColor = true;
        }
    
        // Update current color when the user wants to  
        if(prox.close() && triggerToGetBaseColor){
            if(prox.lastDist() > 90 && prox.lastDist() < 98){
                firstSkinScan(); 
            }
        }
        else if(prox.close() && !triggerToGetBaseColor && !firstRun){
            if(prox.lastDist() > 90 && prox.lastDist() < 98){
                scanSkinColor(); 
            }
        }
    
        //********************************               #2 Color Scanner                  ******************************//
    
        // Update the change in the tanIndex
        tanCompare(currentColor, prevColor);
    }
    
    // #3 Send tan alerts when it's a sunny day, every two hours
    if(sunnyDay() && second == 0 && minutes == 0 && hours % 2 == 0){
        sunAlert(); 
    }
}


// #1.1 Check if the sunglasses are on the stand
bool sunGlassOnStand(){
    if(sunglassesProximity.close()){
        return true;
    }
    else{
        return false;
    }
}


//********************************        #1.2 Check if the the door is open from the inside        ******************************//
bool doorIsOpen()
{
    int num;
    //Check if amount of IR light is greater than threshold
    if (doorProximity.close()) {
        //Say the threshold and that it recieved more IR light than the threshold
        Serial.println("-----------       Door      -----------");
        Serial.print("Close to object, threshold is ");
        Serial.print(String(doorProximity.getThreshold()));
        Serial.print(", distance is ");
        Serial.println(String(doorProximity.lastDist()));
        return true;
    } else {
        //Say the threshold and that it recieved less IR light than the threshold
        Serial.println("-----------       Door      -----------");
        Serial.print("Not close to object, threshold is ");
        Serial.print(String(doorProximity.getThreshold()));
        Serial.print(", distance is ");
        Serial.println(String(doorProximity.lastDist()));
        return false;
    }
    //If user sent data
    if (Serial.available()) {
        //Get number that user sent
        num = Serial.parseInt();
        //Set new threshold
        doorProximity.setThreshold(num);
        //Say the new threshold
        Serial.println("-----------       Door      -----------");
        Serial.print("Changed threshold to ");
        Serial.println(num);
    }
}
//********************************        #1.2 Check if the the door is open from the inside        ******************************//


// #1.3 Checks if there is sun outside
bool sunnyDay(){
    // checks if night time
    if(!dayTime(hours))
    {
        return false;
    }
    Blynk.virtualWrite(V5,99);
    if(weatherStatus.equals("Clear")){
        return true;
    }
    return false;
}


// #1.3 Return true if day time
bool dayTime(int hours) 
{
    if(6 < hours && hours < 18)
    {
        return true;
    }
    return false;
}


// #1.4 Reminds you to take the sunglasses on a sunny day
void takeSunglassesNotification(){
    // Send reminder to take sunglasses
    Blynk.virtualWrite(V0,99);
    
    // Activate Door CPX
    for(int i = 0; i < 9; i++){
        CircuitPlayground.setPixelColor(i, 0, 70, 158); 
        CircuitPlayground.setPixelColor(i+1, 0, 70, 158); 
        delay(20);
        CircuitPlayground.setPixelColor(i, 0, 0, 0); 
    }
    
    CircuitPlayground.playTone(1000,50); 
    CircuitPlayground.playTone(800,50); 
    CircuitPlayground.playTone(900,50); 
    delay(250);
    CircuitPlayground.clearPixels();
}


// #2 UX for the user - can scan his skin now
void buttonPressed(){
    for(int i = 0; i < 9; i += 2){
        CircuitPlayground.setPixelColor(i, 0, 128, 128);
        if(i > 0){
            CircuitPlayground.setPixelColor(i-1, 0, 0, 0);
        }
        CircuitPlayground.playTone(100*i+200,50);
        delay(20);
        CircuitPlayground.setPixelColor(i+1,  16, 65, 98); 
        CircuitPlayground.playTone(100*i+200,50);
        delay(20);
        CircuitPlayground.setPixelColor(i,  0, 0, 0);
    }
    CircuitPlayground.clearPixels();
    delay(1000);
    CircuitPlayground.setPixelColor(5,  200, 200, 200);
}


// #2 Gets the basic tan color and the daily tan color and return tanIndex
void tanCompare(uint8_t* currentColor, uint8_t* prevColor){
    sumTanChange = (prevColor[0] - currentColor[0]) + (prevColor[1] - currentColor[1]) + (prevColor[2] - currentColor[2]);
    int tanChangeLevel = sumTanChange / TAN_INDEX_LEVEL; // Determine the level of tan
    tanIndex += tanChangeLevel;
    // If got tanned
    if(sumTanChange > 0){
        sumTanChange -= tanChangeLevel * TAN_INDEX_LEVEL; // Keep the sheerit of the tan change
    }
    else{
        sumTanChange += tanChangeLevel * TAN_INDEX_LEVEL; // Keep the sheerit of the tan change
    }
    
    // Set min/max tan levels
    if(tanIndex < 1){
        tanIndex = 1;
    }
    else if(tanIndex > 15){
        tanIndex = 15;
    }
}


// #2 When the user scans his basic skin color (tan free, e.g. waist area)
void firstSkinScan(){

    // Reset previous colors
    Blynk.virtualWrite(V2,0);
    Blynk.virtualWrite(V3,0);
    Blynk.virtualWrite(V4,0);
    
    takeSkinColor(basicColor);
    prevColor[0] = basicColor[0];
    prevColor[1] = basicColor[1];
    prevColor[2] = basicColor[2];
    triggerToGetBaseColor = false;

    Serial.println("-----------       Colors      -----------");
    Serial.print("basic RED color=");
    Serial.println(basicColor[0]);
    Serial.print("basic GREEN color=");
    Serial.println(basicColor[1]);
    Serial.print("basic BLUEcolor=");
    Serial.println(basicColor[2]);
    Serial.print("prevColor: ");
    Serial.println(*prevColor);
    
    // Set the led blynk widget at skin's scanned color    
    ledActivator(basicColor[0],basicColor[1],basicColor[2], 3);
    firstRun = false;
    CircuitPlayground.clearPixels();
}


// #2 Scan the skin color based when the hand is positioned next to the cpx 
void scanSkinColor(){
    takeSkinColor(currentColor);
    // Set the led blynk widget at skin's scanned color    
    ledActivator(currentColor[0],currentColor[1],currentColor[2], 2);
    delay(750);
    ledActivator(prevColor[0],prevColor[1],prevColor[2], 4);
    Serial.println("-----------       Colors      -----------");
    Serial.print("prev RED color=");
    Serial.println(prevColor[0]);
    Serial.print("prev GREEN color=");
    Serial.println(prevColor[1]);
    Serial.print("prev BLUEcolor=");
    Serial.println(prevColor[2]);
    Serial.print("current RED color=");
    Serial.println(currentColor[0]);
    Serial.print("current GREEN color=");
    Serial.println(currentColor[1]);
    Serial.print("current BLUEcolor=");
    Serial.println(currentColor[2]);   
    prevColor[0] = currentColor[0];
    prevColor[1] = currentColor[1];
    prevColor[2] = currentColor[2];
}


// #2 Scan the skin color and display it on blynk UI led widget
void takeSkinColor(uint8_t color[3])
{
    int num;
    // Say the threshold and that it recieved more IR light than the threshold
    Serial.print("Close to object, threshold is ");
    Serial.print(String(prox.getThreshold()));
    Serial.print(", distance is ");
    Serial.println(String(prox.lastDist()));
    CircuitPlayground.senseColor(color[0], color[1], color[2]);
    CircuitPlayground.playTone(100,50);
    CircuitPlayground.playTone(800,50);
    delay(500);
    
    //If user sent data
    if(Serial.available()){
        //Get number that user sent
        num = Serial.parseInt();
        //Set new threshold
        prox.setThreshold(num);
        //Say the new threshold
        Serial.print("Changed threshold to ");
        Serial.println(num);
    }
}


// #2 Activate the blynk led according to the skin's current color
void ledActivator(int r, int g, int b, int ledNum){
    String rH = decToHexa(r);
    Serial.print("rH now: ");
    Serial.println(rH);
    String gH = decToHexa(g);
    Serial.print("gH now: ");
    Serial.println(gH);
    String bH = decToHexa(b);
    Serial.print("bH now: ");
    Serial.println(bH);
    String num = "#" + rH + gH + bH;
    Serial.print("Concatenated num: ");
    Serial.println(num);
    if(ledNum == 2){
        Blynk.virtualWrite(V2, 255);
        Blynk.setProperty(V2, "color", num);  
    }
    else if(ledNum == 3){
        Blynk.virtualWrite(V3, 255);
        Blynk.setProperty(V3, "color", num);  
    }
    else if(ledNum == 4){
        Blynk.virtualWrite(V4, 255);
        Blynk.setProperty(V4, "color", num);  
    }
    delay(1000);
}


// #2 Convert decimal to hexadecimal represented by a string
String decToHexa(int n) 
{    
    // Char array to store hexadecimal number 
    char hexaDeciNum[2] = {'0','0'};
      
    // counter for hexadecimal number array 
    int i = 0; 
    
    while(n != 0) {    
        
        // Temporary variable to store remainder 
        int temp  = 0; 
          
        // storing remainder in temp variable. 
        temp = n % 16; 
          
        // check if temp < 10 
        if(temp < 10) 
        { 
            hexaDeciNum[i] = temp + 48; 
            i++; 
        } 
        else
        { 
            hexaDeciNum[i] = temp + 55; 
            i++; 
        } 
          
        n = n/16; 
    } 
    Serial.println("-----------       DecToHexa      -----------");
    Serial.print("Real Number ");
    Serial.print(hexaDeciNum[1]);
    Serial.print(" ");
    Serial.println(hexaDeciNum[0]);
    char hexNum[2]; 
    hexNum[1] = hexaDeciNum[0];
    hexNum[0] = hexaDeciNum[1];
    Serial.print("After char transform ");
    Serial.println(hexNum);
    return hexNum; 
} 


// #3 Send sun alert to the user based on his tanIndex
void sunAlert(){
    // Choose the appropriate tan recommendation
    int tanRecommend = tanIndex / 3;
    recommendation = recommendTanning[tanRecommend];   
    
    // Send to blynk webhook to trigger android notification
    Blynk.virtualWrite(V1, recommendation);
}


// Reads the weather status from virtual pin V5 (gets the information from integromat)
BLYNK_WRITE(V5){
    // Reading the weather status
    weatherStatus = param[0].asString();
    Serial.println("-----------       Weather      -----------");
    Serial.print("Weather status is: ");
    Serial.println( weatherStatus);
}


// Checks if the user left or entered his home to activate / deactivate the system
BLYNK_WRITE(V6){
    // Reading the value
    isHome = param[0].asInt();
}


// Clock function
void basicClock(){
    // Add one second every loop
    second++;
    
    // Add one minute every 60 minutes
    if(second>=60)
    {
        second=0;
        minutes++;
    }
    
    // Add one hour every 60 minutes 
    if(minutes>=60)
    {
        second=0;
        minutes=0;
        hours++;
    }
    
    // Verify if hours more then 24
    if(hours>=24)
    {
        second=0;
        minutes=0;
        hours=0;
    }
    
    Serial.println("-----------       Clock      -----------");
    Serial.print("Time : ");
    //print the time
    Serial.print(hours);
    //Serial.print(" minutes: ");
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(second);
}
