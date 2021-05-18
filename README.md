# IoT
IoT projects to automate daily tasks including HW & SW.
Every project is basically a smart brain which connects real life experiences with automated responses, which all together provide a simpler way to live.  
Actions like activating the boilermaker, lights & TV, or waking up in the morning become more elegant & require less effort.
The data is presented via smart dashboards.

<img src="https://github.com/BenHurCreations/IoT/blob/main/Images/Connect.jpg" alt="Connect" width="500" height="250"/>

## Technologies
1. Arduino - development environment 
<img src="https://github.com/BenHurCreations/IoT/blob/main/Images/Arduino.jpg" alt="arduino" width="120" height="60"/>

2. C++ - Coding language
3. Adafruit Circuit Playground (CPX) - sensors & real world interactions
<img src="https://github.com/BenHurCreations/IoT/blob/main/Images/CPX.jpg" alt="CPX" width="150" height="115"/>

4. Blynk - widgets world + wifi connectivity
<img src="https://github.com/BenHurCreations/IoT/blob/main/Images/Blynk.png" alt="Blynk" width="100" height="40"/>


5. Integromat - automating processes
<img src="https://github.com/BenHurCreations/IoT/blob/main/Images/Integromat.png" alt="Integromat" width="170" height="50"/>

# Projects
## #1 Electro Singer
**Electro Singer** is our first device built with adafruit circuit playground.  
It uses ports A1-A5 to detect tapping on 5 different veggies-  
avocado, guyava, clemantine, banana & potato!

- Each tap changes the capacitivity values of the sensors and triggers a relevant sound.  
- We used the light sensor to determine the music tone.  
- With regular light we have medium tone. With extra light we have higher tone,   
and with little light we have low tone.  
- The second output is led light. Any tap the code produces a different color.  

We chose to play "Little Yonatan", the beloved children's song.

  ### The circuit:
  * Ports A1-A5 are attached to the drum-fruits by alligators
  * The change in capacitivity in A1-A5 capacitivity sensors triggers the relevant sound
  * The amount of light determines the music tone

**Video link:** https://youtu.be/rAO612zG3VQ

## #2 MiDay
**MiDay** is a light clock whick contatins different features for the user.  
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
The clock continues to work, and the user is asked if he slept good or not. He answers using the rught & left buttons.  
This data + the time it took to the user to wake up from bed and finish the joystick challenge is stored for future analysis.  
  
  ### The circuit:
  * Ports A1-A2 are attached to extra leds.  
  * Port A3 is connected to the touch surface.  
  * The change in capacitivity in A3 capacitivity sensors helps counting how many times the user got out from his bed during the night  
  * Ports A0, A4, A5, 3.3V, GND are connected to the joystick.   
  * 3.3v & GND for power, A0 for the x axis of the joystick, A4 for the y axis & A5 for the Switch Pin of the joystick.  

## #3 Smart Homie
**SmartHomie** is a four widgets based system that enables the user control remotely different aspects of his life.

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
  
  Video link: https://youtu.be/NcAQs3vaNF4  
    
## Final Project

**Tanny**- your personal tan manager.  
  
A detailed guide was made using instructables plaform.  
The guide includes DIY instructions, project deep explanation, technologies involved & a great documentaion video.  
Please follow the link to the guide ---> https://www.instructables.com/Tanny-Tan-Manager/

<img src="https://github.com/BenHurCreations/IoT/blob/main/Images/Final.PNG" alt="final" width="500" height="275"/>  
  
    
**Special thanks** to Gal Kimron & Hadar Okashi
