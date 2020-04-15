#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include <frequencyToNote.h>
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 7 // Popular NeoPixel ring siz
#define PIN        2


#define NOTE_B0  23
#define NOTE_C1  24
#define NOTE_CS1 25
#define NOTE_D1  26
#define NOTE_DS1 27
#define NOTE_E1  28
#define NOTE_F1  29
#define NOTE_FS1 30
#define NOTE_G1  31
#define NOTE_GS1 32
#define NOTE_A1  33
#define NOTE_AS1 34
#define NOTE_B1  35
#define NOTE_C2  36
#define NOTE_CS2 37
#define NOTE_D2  38
#define NOTE_DS2 39
#define NOTE_E2  40
#define NOTE_F2  41
#define NOTE_FS2 42
#define NOTE_G2  43
#define NOTE_GS2 44
#define NOTE_A2  45
#define NOTE_AS2 46
#define NOTE_B2  47
#define NOTE_C3  48
#define NOTE_CS3 49
#define NOTE_D3  50
#define NOTE_DS3 51
#define NOTE_E3  52
#define NOTE_F3  53
#define NOTE_FS3 54
#define NOTE_G3  55
#define NOTE_GS3 56
#define NOTE_A3  57
#define NOTE_AS3 58
#define NOTE_B3  59
#define NOTE_C4  60
#define NOTE_CS4 61
#define NOTE_D4  62
#define NOTE_DS4 63
#define NOTE_E4  64
#define NOTE_F4  65
#define NOTE_FS4 66
#define NOTE_G4  67
#define NOTE_GS4 68
#define NOTE_A4  69
#define NOTE_AS4 70
#define NOTE_B4  71
#define NOTE_C5  72
#define NOTE_CS5 73
#define NOTE_D5  74
#define NOTE_DS5 75
#define NOTE_E5  76
#define NOTE_F5  77
#define NOTE_FS5 78
#define NOTE_G5  79
#define NOTE_GS5 80
#define NOTE_A5  81
#define NOTE_AS5 82
#define NOTE_B5  83
#define NOTE_C6  84
#define NOTE_CS6 85
#define NOTE_D6  86
#define NOTE_DS6 87
#define NOTE_E6  88
#define NOTE_F6  89
#define NOTE_FS6 90
#define NOTE_G6  91
#define NOTE_GS6 92
#define NOTE_A6  93
#define NOTE_AS6 94
#define NOTE_B6  95
#define NOTE_C7  96
#define NOTE_CS7 97
#define NOTE_D7  98
#define NOTE_DS7 99
#define NOTE_E7  100
#define NOTE_F7  101
#define NOTE_FS7 102
#define NOTE_G7  103
#define NOTE_GS7 104
#define NOTE_A7  105
#define NOTE_AS7 106
#define NOTE_B7  107
#define NOTE_C8  108

#define REST 0

float rawVal[2] = {0} ;
float currentVal[2] = {0};
float lastVal[2] = {0};
float weight;
float peakVal[2] = {0};
int threshold = 100;   //set your own value based on your sensors
int noise = 15;        //set a noise value based on your particular sensor


Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup(){
  weight = 0.3;
  pinMode(13,OUTPUT);
  pixels.begin();
   Serial.begin(9600);
   for(int i = 0; i < NUMPIXELS; i++){
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();
    pixels.setBrightness(255);
   }
}

void loop(){

  for (int i = 0; i < 2; i++){
    if(i ==0){
      rawVal[i] = analogRead(A0);
    }else{
      rawVal[i] = analogRead(A1);
    }
    
    currentVal[i] = filter(rawVal[i],weight,lastVal[i]);

    if (currentVal[i] > peakVal[i]) {
      peakVal[i] = currentVal[i];
    }

  if(currentVal[i] <= threshold - noise){
    if(peakVal[i] > threshold + noise){
//      digitalWrite(13, HIGH);
      if(i == 0){
        midiCmd(0x90, NOTE_C5, 127);
        delay(50);
        midiCmd(0x80, NOTE_C5, 127);
      }else{
        Serial.println(currentVal[i]);
        midiCmd(0x91, NOTE_C4, 127);
        delay(50);
        midiCmd(0x81, NOTE_C4, 127);
      }
      
      peakVal[i] = 0;
    }else{
      if(i==0){
         midiCmd(0x80, NOTE_C5, 0x00);
        delay(50)
        ;
       midiCmd(0x80, NOTE_C5, 127);
      }else{
         midiCmd(0x81, NOTE_C4, 0x00);
      delay(50);
       midiCmd(0x81, NOTE_C4, 127);
      }
     
//      digitalWrite(13, LOW);    
    }
    }
  
  lastVal[i] = currentVal[i];  
  
  }
//  rawVal[0] = analogRead(A0);
//  currentVal[0] = filter(rawVal[0],weight,lastVal[0]);
////  currentVal[1] = filter(rawVal2,weight,lastVal2);
//  Serial.print(0);  // To freeze the lower limit
//  Serial.print(" ");
//  Serial.print(256);  // To freeze the upper limit
//  Serial.print(" ");
//  Serial.println(currentVal[0]);
//
//  if (currentVal[0] > peakVal[0]) {
//    peakVal[0] = currentVal[0];
//  }
//
//  if(currentVal[0] <= threshold - noise){
//    if(peakVal[0] > threshold + noise){
//      digitalWrite(13, HIGH);
//      midiCmd(0x90, NOTE_C5, 127);
//      delay(50);
//      midiCmd(0x80, NOTE_C5, 127);
//      peakVal[0] = 0;
//    }else{
//      midiCmd(0x80, NOTE_C5, 0x00);
//      delay(50);
//       midiCmd(0x80, NOTE_C5, 127);
//      digitalWrite(13, LOW);    
//    }
//  }
//  
//  lastVal[0] = currentVal[0];  

  
}

float filter(float rawVal, float weight, float lastVal){
  float result = weight*rawVal + (1-weight)*lastVal;
  return result;
}

void midiCmd(byte cmd, byte data1, byte data2){
  midiEventPacket_t midiMsg = {cmd >>4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
}
