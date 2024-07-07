  ///https://youtu.be/w1XMInQThNI(Please watch and Subscribe )

#include <DS3232RTC.h>
#include <TimeLib.h>
#include <Time.h>
#include <Wire.h>
#include <FastLED.h>


#define NUM_LEDS 114 // 4*7*4 +2  Number of LED controlled
#define COLOR_ORDER GRB  // Define color order for your strip
#define LED_PIN 9 // Data pin for led comunication
#define DST_PIN 5  // Define DST adjust button pin
#define MIN_PIN 4  // Define Minutes adjust button pin
#define HUR_PIN 2  // Define Hours adjust button pin
#define BRI_PIN 3 // Define Light sensor pin
CRGB leds[NUM_LEDS]; // Define LEDs strip
                    // 0,0,0,0
                    // 1,1,1,1
                    //  1 2 3 4 5 6 7 8 9 10111213141516171819202122232425262728
byte digits[12][28] = {{0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},   // Digit 0
                       {0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},   // Digit 1
                       {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},   // Digit 2
                       {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},   // Digit 3
                       {1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1},   // Digit 4
                       {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1},   // Digit 5
                       {1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},   // Digit 6
                       {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},   // Digit 7
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},   // Digit 8
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1},   // Digit 9 | 2D Array for numbers on 7 segment
                       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},   // Digit *0
                       {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0}};  // Digit C
                       
                       
bool Dot = true;  //Dot state
bool DST = true; //DST state// false to true
bool TempShow = false;
int last_digit = 0;

// int ledColor = 0x0000FF; // Color used (in hex)
long ledColor = CRGB::DarkOrchid; // Color used (in hex)
//long ledColor = CRGB::MediumVioletRed;
//Random colors i picked up
long ColorTable[21] = {
  CRGB::Amethyst,  //white
  CRGB::Aqua,    //pink
  CRGB::Blue,     //Blue
  CRGB::Chartreuse,// Gold
  CRGB::DarkGreen, //Red
  CRGB::DarkMagenta,//Aqua
  CRGB::DarkOrange,  // yellow green
  CRGB::DeepPink,  //Aqua
  CRGB::Fuchsia,  //Sea blue
  CRGB::Gold,    //Gold
  CRGB::GreenYellow,//off white
  CRGB::LightCoral,//white
  CRGB::Tomato,//white
  CRGB::Salmon,//Pure white
  CRGB::Red,// Drak Green
  CRGB::Orchid,//blue white
  CRGB::Sienna,//yellow white
  CRGB::Purple,// aqua
  CRGB::DarkOrange,//yellow green
  CRGB::FloralWhite,//white
  CRGB::Yellow  //yellow
};


void setup(){ 
  Serial.begin(9600); 
  Wire.begin();
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);
  LEDS.setBrightness(20); // Set initial brightness
  pinMode(DST_PIN, INPUT_PULLUP); // Define DST adjust button pin
  pinMode(MIN_PIN, INPUT_PULLUP); // Define Minutes adjust button pin
  pinMode(HUR_PIN, INPUT_PULLUP); // Define Hours adjust button pin
  pinMode(BRI_PIN, INPUT_PULLUP); // Define bright adjust 
  TempShow = false; // do not show temperature 
} 

// Get time in a single number, if hours will be a single digit then time will be displayed 155 instead of 0155
int GetTime(){
  tmElements_t Now;
  RTC.read(Now);
  //time_t Now = RTC.Now();// Getting the current Time and storing it into a DateTime object 
  int hour=Now.Hour;
  int minute=Now.Minute;
  int second =Now.Second;
  if (second % 2==0) {Dot = false;}
    else {Dot = true;};
 
 if (hour >= 12) {
 hour -= 12;
  }
 // Handle hour 0 (midnight) being shown as 12.
  // else if (hour == 0) {
  // hour += 12;
  // }
     return (hour*100+minute);
  };

 // Check Light sensor and set brightness accordingly
void BrightnessCheck(){
  const byte sensorPin = 3; // light sensor pin
  const byte brightnessLow = 75; // Low brightness value
  const byte brightnessHigh = 20; // High brightness value
  int sensorValue = digitalRead(sensorPin); // Read sensor
  if (sensorValue == 0) {LEDS.setBrightness(brightnessHigh);}
  else {LEDS.setBrightness(brightnessLow);} 
  }; 
// Convert time to array needed for display 
void TimeToArray(){
  int Now = GetTime();  // Get time
  
  int cursor = 114; // last led number
  
  // Serial.print("Time is: ");Serial.println(Now);
  if (DST){   // if DST is true then add one hour
   Now+=100;
  // Serial.print("DST is ON, time set to : ");Serial.println(Now);
  }; 
  if (Dot){leds[57]=ledColor;
           leds[56]=ledColor;}   
    else  {leds[57]=0x000000;
           leds[56]=0x000000;
    };
    
  for(int i=1;i<=4;i++){
    int digit = Now % 10; // get last digit in time
    if (i==1){
    //  Serial.print("Digit 4 is : ");Serial.print(digit);Serial.print(" ");

      cursor = 86;
      
      for(int k=0; k<=27;k++){ 
        // Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      // Serial.println();

      if (digit != last_digit)
      {
        cylon();
        ledColor =  ColorTable[random(21)];
      }
      last_digit = digit;
      
      }
    else if (i==2){
      // Serial.print("Digit 3 is : ");Serial.print(digit);Serial.print(" ");

      cursor =58;
      
      for(int k=0; k<=27;k++){ 
        // Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      // Serial.println();
      }
     else if (i==3){
      // Serial.print("Digit 2 is : ");Serial.print(digit);Serial.print(" ");
      cursor =28;
      for(int k=0; k<=27;k++){ 
        // Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      // Serial.println();
      }
    else if (i==4){
      // Serial.print("Digit 1 is : ");Serial.print(digit);Serial.print(" ");
      cursor =0;
      if(digit !=0){
         for(int k=0; k<=27;k++){ 
        // Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
    }
      if(digit ==0){
      for(int k=0; k<=27;k++){ 
        // Serial.print(digits[digit][k]);
        if (digits[12][k]== 1){leds[cursor]=ledColor;}
         else if (digits[12][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      
      // Serial.println();
      }
    }
     Now /= 10;
  }; 
};

// Convert temp to array needet for display 
void TempToArray(){
  tmElements_t tm;
  RTC.read(tm);

  if (tm.Second != 27) {
    TempShow = false;
    return;
  }
  TempShow = false;//true to false
  int t = RTC.temperature();
  int celsius = (t / 4.0) * 100;

  Serial.print("Temp is: ");Serial.println(celsius);
  
  int cursor = 114; // last led number
    
    leds[42]=0x000000;
    leds[43]=0x000000;
      
  for(int i=1;i<=4;i++){
    int digit = celsius % 10; // get last digit in time
    if (i==1){
      Serial.print("Digit 4 is : ");Serial.print(digit);Serial.print(" ");

      cursor = 86;
      
      for(int k=0; k<=27;k++){ 
        Serial.print(digits[11][k]);
        if (digits[11][k]== 1){leds[cursor]=ledColor;}
         else if (digits[11][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
    }
    else if (i==2){
      Serial.print("Digit 3 is : ");Serial.print(digit);Serial.print(" ");

      cursor =58;
      
      for(int k=0; k<=27;k++){ 
        Serial.print(digits[10][k]);
        if (digits[10][k]== 1){leds[cursor]=ledColor;}
         else if (digits[10][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
      }
    else if (i==3){
      Serial.print("Digit 2 is : ");Serial.print(digit);Serial.print(" ");
      cursor =28;
      for(int k=0; k<=27;k++){ 
        Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
      }
    else if (i==4){
      Serial.print("Digit 1 is : ");Serial.print(digit);Serial.print(" ");
      cursor =0;
      for(int k=0; k<=27;k++){ 
        Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
      }
    celsius /= 10;
  }; 
};


void DSTcheck(){
   int buttonDST = digitalRead(5);
   // Serial.print("DST is: ");Serial.println(DST);
   if (buttonDST == LOW){
    if (DST){
      DST=false;
      // Serial.print("Switching DST to: ");Serial.println(DST);
      }
      else if (!DST){
        DST=true;
        // Serial.print("Switching DST to: ");Serial.println(DST);
      };
   delay(500);   
   };
  }

void TimeAdjust(){
  int buttonH = digitalRead(HUR_PIN);
  int buttonM = digitalRead(MIN_PIN);
  if (buttonH == LOW || buttonM == LOW){
    delay(500);
    tmElements_t Now;
    RTC.read(Now);
    int hour=Now.Hour;
    int minutes=Now.Minute;
    int second =Now.Second;
      if (buttonH == LOW){
        if (Now.Hour== 23){Now.Hour=0;}
          else {Now.Hour += 1;};
        }else {
          if (Now.Minute== 59){Now.Minute=0;}
          else {Now.Minute += 1;};
         // if (Now.Hour > 12){Now.Hour = Now.Hour -12;}  // 24 to 12 hr  
         // else {Now.Hour = Now.Hour ;}; 
          };
    RTC.write(Now); 
    }
  }

/* coool effect function*/
void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }


void cylon () {
  static uint8_t hue = 0;
  Serial.print("x");
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  Serial.print("x");

  // Now go in the other direction.  
  for(int i = (NUM_LEDS)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}
  
void loop()  // Main loop
{
  BrightnessCheck(); // Check brightness
  DSTcheck(); // Check DST
  TimeAdjust(); // Check to se if time is geting modified
  TimeToArray(); // Get leds array with required configuration
  TempToArray(); // Show Temperature
  FastLED.show(); // Display leds array
  if (TempShow == true) delay (8000);
}
