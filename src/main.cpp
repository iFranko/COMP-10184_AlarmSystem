// COMP-10184 – Mohawk College 
// Application COMP-10184_AlarmSystem 
//  
// This program demonstrate the use of alarm system using motion detector sensor and buttons
// 
// @author  Frank Kufer 
// @id   Student 000805328 
// 
// I created this work and I have not shared it with anyone else. 
// 


#include <Arduino.h>
// digital input pin definitions 
#define PIN_PIR D5 
#define PIN_BUTTON D6 
#define DELAY 10000
#define BUTTON_DELAY 200  
#define ALARM_DISABLED  0 
#define ALARM_ENABLE    1 
#define ALARM_COUNTDOWN 2 
#define ALARM_ACTIVE    3 

int  iAlarmState = ALARM_ENABLE;
int  iCOUNT =0;
int  iButton;
bool bBUTTON_STATE=false;
int  iCLICKED_ONCE =0;
bool bDELAY_SWITCH=true;


// ************************************************************* //functions 

//check alarm state
int checkAlarmState(){

    switch (iAlarmState) {
    case 0:
      return ALARM_DISABLED;
      break;
    case 1:
      return ALARM_ENABLE;
      break;
    case 2:
      return ALARM_COUNTDOWN;
      break;
    default:
      return ALARM_ACTIVE;
      break;
    }

}

//check button if pressed 
void PressButton(int iButton){
   Serial.println(iButton);
  if(iButton==0 && iCLICKED_ONCE==0){
    iCLICKED_ONCE=1;
    
    if(checkAlarmState()==ALARM_ENABLE || checkAlarmState()==ALARM_COUNTDOWN){
      digitalWrite(LED_BUILTIN, true);
      delay(BUTTON_DELAY);
      iAlarmState=ALARM_DISABLED;
      iCOUNT=0;
      bBUTTON_STATE=true;
      
    }
    else if(checkAlarmState()==ALARM_DISABLED){
      delay(BUTTON_DELAY);
      bBUTTON_STATE=false;
      iAlarmState=ALARM_ENABLE;
      iCOUNT=0;
    }
    
  }
  if(iButton==1 ){
    iCLICKED_ONCE=0;
  }

}


//blink 4 times in a second
void FourBlinks() { 
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(125);                        // wait for a 0.125 second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(125);                        // wait for a 0.125 second
}

// Stage 1
void StageOne(bool bPIR) { 
  if (bPIR) {            // if motion detected
    digitalWrite(LED_BUILTIN, !bPIR); // turn LED ON
    
    delay(DELAY);
  } 

  // send the PIR signal directly to the LED 
  // but invert it because true = LED off! 
  
  else {
    digitalWrite(LED_BUILTIN, !bPIR);  // turn LED OFF if we have no motion
  }
  
} 


//************ Stage 2 
void StageTwo(bool bPIR) { 
  // if motion detected
  if (bPIR) {
    //active the alaram if it is enabled
    if(checkAlarmState()==ALARM_ENABLE) {
      iAlarmState=ALARM_ACTIVE;
    }      
  } 
  //if the alarm is active blinkthe LED 4 time in second for 10 seconds
  if(checkAlarmState()==ALARM_ACTIVE){
    FourBlinks();
    // count up till 10 seconds then exist the loop
    iCOUNT+=250;
  }

  // once it is 10 seconds switch alarm to enable and icount to 0
  if (iCOUNT==DELAY || checkAlarmState()==ALARM_ENABLE) {
    digitalWrite(LED_BUILTIN, true);  // turn LED OFF if we have no motion
    iAlarmState=ALARM_ENABLE;
    iCOUNT=0;
  } 
  
}


//************ Stage 3
void StageThree(bool bPIR) { 

  // if motion detected
  if (bPIR) {
    //if alaram is enabled then activte the alarm
     if(checkAlarmState()==ALARM_ENABLE) {
      iAlarmState=ALARM_ACTIVE;
    }  
  } 
  // if alarm is active  start 10 seconds count down and true on solid LED
  if(checkAlarmState()==ALARM_ACTIVE){
    delay(DELAY);
    digitalWrite(LED_BUILTIN, false);
  }
  //if alarm is enable which it is on initial trun off the LED
  else if(checkAlarmState()==ALARM_ENABLE){
    digitalWrite(LED_BUILTIN, true);  // turn LED OFF if we have no motion
    iAlarmState=ALARM_ENABLE;
  }
}

//************ Stage 4
void StageFour(bool bPIR, int iButton) { 

  //when button is pressed 
  if(iButton==0 && iCLICKED_ONCE==0){
    bBUTTON_STATE=true;
    iCLICKED_ONCE=1;
  }
  //when button is not pressed
  if(iButton==1 ){
    iCLICKED_ONCE=0;
  }
  // if motion detected
  if (bPIR) {
    //if alaram is enabled then activted the alarm
      if(checkAlarmState()==ALARM_ENABLE) {
        iAlarmState=ALARM_ACTIVE;
    }  
  } 
  // if alarm is active start 10 seconds count down then start blinkin the LED
  if(checkAlarmState()==ALARM_ACTIVE){
    if(bDELAY_SWITCH){
      delay(DELAY); 
      bDELAY_SWITCH =false;
    }
    FourBlinks();
  }


  // if button state is true or alarm in enabled then turn off the LED 
  if(bBUTTON_STATE || checkAlarmState()==ALARM_ENABLE){
    digitalWrite(LED_BUILTIN, true);  // turn LED OFF if we have no motion
    iAlarmState=ALARM_ENABLE;
    bBUTTON_STATE=false;
    bDELAY_SWITCH=true;
    
  }
}

//************ Stage 5 
void StageFive(bool bPIR) { 

  iButton = digitalRead(PIN_BUTTON);
   
  
  // if motion detected
  if (bPIR) {
    //if alarm is enabled set it to active
    if(checkAlarmState()==ALARM_ENABLE){
       iAlarmState=ALARM_ACTIVE;
    }
  } 
  //if alarm is active set it to countdown and start 10 seconds count down
  if(checkAlarmState()==ALARM_ACTIVE){

    iAlarmState=ALARM_COUNTDOWN;
    // start blinking the LED for 10 seconds if a button is pressed turn it off otherwise set the alarm ON
    while(checkAlarmState()==ALARM_COUNTDOWN && iCOUNT < DELAY && !bBUTTON_STATE){
      FourBlinks();
      iCOUNT+=250;
      iButton = digitalRead(PIN_BUTTON);
      PressButton(iButton);

    }
  }
  // when a button is not pressed within 10 seconds 
  if(checkAlarmState()==ALARM_COUNTDOWN && iCOUNT == DELAY){
    digitalWrite(LED_BUILTIN, false);  // turn LED OFF if we have no motion
  }
  //if button is pressed turn off teh alram
  else if(checkAlarmState()==ALARM_DISABLED || checkAlarmState()==ALARM_ENABLE){
    //call buttn function to switch the alarm state
     PressButton(iButton);
     digitalWrite(LED_BUILTIN, true);
     
  }
}


// ************************************************************* 
void setup() { 
  // configure the USB serial monitor 
  Serial.begin(115200); 
 
  // configure the LED output 
  pinMode(LED_BUILTIN, OUTPUT); 
 
  // PIR sensor is an INPUT 
  pinMode(PIN_PIR, INPUT); 
 
  // Button is an INPUT 
  pinMode(PIN_BUTTON, INPUT_PULLUP); 

} 
 
// ************************************************************* 
void loop() { 
  bool bPIR; 
  iButton = digitalRead(PIN_BUTTON);
  // read PIR sensor (true = Motion detected!).  As long as there 
  // is motion, this signal will be true.  About 2.5 seconds after  
  // motion stops, the PIR signal will become false. 
  bPIR = digitalRead(PIN_PIR); 
 

  // StageOne(bPIR);            //stage 1 
  
  // StageTwo(bPIR);             //Stage 2 

  // StageThree(bPIR);          //Stage 3
 
  // StageFour(bPIR, iButton);   //Stage 4

  StageFive(bPIR);              //Stage 5

  
} 

   


