#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include <frequencyToNote.h>
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 8 // Popular NeoPixel ring siz
#define PIN    9    
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

float rawVal[7] = {0};
float currentVal[7] = {0};
float lastVal[7] = {0};
float weight;
float peakVal[7] = {0};
int threshold = 180;
int noise = 10;
boolean light[7] = {false};
boolean lastLight[7] = {false};
//unsigned long CM;
unsigned long previousMillis = 0;  
const long interval = 100;   


void setup() {
  // put your setup code here, to run once:
  weight = 0.3;
  pinMode(13,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  Serial.begin(9600);
//  pixels.begin();
//  pixels.setBrightness(255);
//  for(int i = 0; i < NUMPIXELS; i++){
//    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
//  }
//  pixels.show();  
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
//  for(int i = 0; i < NUMPIXELS; i++){
//    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
//    pixels.show();
//   }

  
  int index = 0;
   for (int whichPort = A0; whichPort <= A6; whichPort++){
//     Serial.print ("Analog port = ");
//     Serial.print (whichPort);
 //    analogRead (whichPort);  // dummy read
     rawVal[index] = analogRead (whichPort);
//     Serial.print (", result = ");
//     Serial.println (rawVal[index]);
     currentVal[index] = filter(rawVal[index], weight, lastVal[index]);

     if(currentVal[index] > peakVal[index]){
      peakVal[index] = currentVal[index];
     }

     if(currentVal[index] <= threshold - noise){
        if(peakVal[index] > threshold + noise){
          light[index] = true;
        }
        peakVal[index] = 0;
      }else{
        light[index] = false;
      }
      Serial.println (light[index]);
      lastVal[index] = currentVal[index];
      

      index++;
   }


// output integration
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    
    for (int i = 0; i < sizeof(light); i++){
      if(light[i] != lastLight[i] && light[i]){
        switch (i) {
          case 0:
            midiCmd(0x90, pitchC4, 127);
            break;
          case 1:
            midiCmd(0x91, pitchD4, 127);
            break;
          case 2:
            midiCmd(0x92, pitchE4, 127);
            break;
          case 3:
            midiCmd(0x93, pitchF4, 127);
            break;
          case 4:
            midiCmd(0x94, pitchG4, 127);
            break;
          case 5:
            midiCmd(0x95, pitchA4, 127);
            break;
          case 6:
            midiCmd(0x96, pitchB4, 127);
            break;
          default:;
        }
        digitalWrite(8-i,HIGH);
//        lastLight[i] = light[i];
     }else{    
      digitalWrite(8-i,LOW);
      switch (i) {
          case 0:
            midiCmd(0x80, pitchC4, 0x00);
            break;
          case 1:
            midiCmd(0x81, pitchD4, 0x00);
            break;
          case 2:
            midiCmd(0x82, pitchE4, 0x00);
            break;
          case 3:
            midiCmd(0x83, pitchF4, 0x00);
            break;
          case 4:
            midiCmd(0x84, pitchG4, 0x00);
            break;
          case 5:
            midiCmd(0x85, pitchA4, 0x00);
            break;
          case 6:
            midiCmd(0x86, pitchB4, 0x00);
            break;
          default:;
        }
     }
    } 
  }else{
//    for (int i = 0; i < sizeof(light); i++){
//       switch (i) {
//          case 0:
//            midiCmd(0x80, pitchC4, 0x00);
//            break;
//          case 1:
//            midiCmd(0x81, pitchD4, 0x00);
//            break;
//          case 2:
//            midiCmd(0x82, pitchE4, 0x00);
//            break;
//          case 3:
//            midiCmd(0x83, pitchF4, 0x00);
//            break;
//          case 4:
//            midiCmd(0x84, pitchA4, 0x00);
//            break;
//          default:
//            midiCmd(0x85, pitchB4, 0x00);
//        }
//    }
  }

}


float filter(float rawVal, float weight, float lastVal){
  float result = weight*rawVal + (1-weight)*lastVal;
  return result;
}


void midiCmd(byte cmd, byte data1, byte data2){
  midiEventPacket_t midiMsg = {cmd >>4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
}
