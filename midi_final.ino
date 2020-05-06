#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include <frequencyToNote.h>
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>

// variables of light sensors
float rawVal[7] = {0}; // rawval from light sensors
float currentVal[7] = {0}; // filter to rawval
float lastVal[7] = {0};
float weight;
float peakVal[7] = {0};
// variables to adjust threshold. The bigger threshold is, the more sensitive sensors will be
int init_threshold[7] = {280,280,280,280,380,380,380};
int threshold[7] = {0};
int adjustment = 0;

// variables to detect peaks
int noise = 10;
boolean light[7] = {false};
boolean lastLight[7] = {false};
//unsigned long CM;
unsigned long previousMillis = 0;  
const long interval = 100;




void setup() {
  // put your setup code here, to run once:
  weight = 0.3;
  pinMode(A7,INPUT);
}


void loop() {
  // put your main code here, to run repeatedly:

  // read potentiometer and adjust threshold;
   int rawA = analogRead(A7);
   adjustment = adjust(rawA);
   for (int i = 0; i < 7; i++){ // update thresholds
    threshold[i] = init_threshold[i] - adjustment;
   }

  
  unsigned long currentMillis = millis();


  // read values from 7 sensors and detect peaks
  int index = 0;
   for (int whichPort = A0; whichPort <= A6; whichPort++){
//     Serial.print ("Analog port = ");
//     Serial.print (whichPort);
 //    analogRead (whichPort);  // dummy read
     	rawVal[index] = analogRead (whichPort);
//     Serial.print (", result = ");
//     Serial.println (rawVal[index]);
     currentVal[index] = filter(rawVal[index], weight, lastVal[index]);

     // peak detection
     if(currentVal[index] > peakVal[index]){
      peakVal[index] = currentVal[index];
     }

     if(currentVal[index] <= threshold[index] - noise){
        if(peakVal[index] > threshold[index] + noise){
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


// output integration -- if current peak is detected, sending a midi message
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    
    for (int i = 0; i < sizeof(light); i++){
      if(light[i] != lastLight[i] && light[i]){ // meaning a new note
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
//        
     }else if (light[i] == lastLight[i] && light[i]){ // continuing the current note
      ;
     }
     else{     // turning off note
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
     lastLight[i] = light[i];
    } 
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


int adjust (int rawValue){
  if (rawValue <= 50){
    return 0;
  }else if (rawValue >= 1000){
    return 200;
  }else{
    return map(rawValue,0,1023,0,200);
  }
  
}
