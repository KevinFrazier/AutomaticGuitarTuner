#include "SoftwareSerial.h"

int ledLow=A3;
int noteGood=A2;
int ledHigh=A4;
int tempMode;
int GUITAR_IN=A0;
int peaks = 0;
int sample[660];
float period = 0;
float frequency = 0;
float sampleTime = 0;
float startTime = 0;
float stopTime = 0;
float tempPeriod = 0;
float tempfrequency = 0;
float frequencyArray[6] = {82.41,110,146.83,196.00,246.94,329.63};

int choice = 0;
int alternate = 0;

int TX1 = 2;
int TX2 = 3;

SoftwareSerial myWork1(0,TX1);
SoftwareSerial myWork2(0,TX2);

int hit = 0;

enum FrequencyStates{Start,GetFrequency, TransferData} FrequencyState;
int string;
int closestString;
float minvalue = 0;
void setup()
{
  Serial.begin(230400);
  myWork1.begin(9600);
  myWork2.begin(9600);

pinMode(GUITAR_IN, INPUT); //Set the PIN 28(A05) as and input
}

void loop(){

main();

}

int main(){

  while(1){
    FrequencyTick();
  }
  return 0;
}
void FrequencyTick(){

  //Transitions
  switch(FrequencyState){
    case Start: FrequencyState = GetFrequency; break;
    case GetFrequency: FrequencyState = TransferData; break;
    case TransferData: FrequencyState = Start; break;
  }
  //Actions
  switch(FrequencyState){
    case Start: break;
    case GetFrequency:
      startTime=millis(); //start of sampling
  
      for(int i=0; i<660; i++){ //takes 800 samples of the input
      
          sample[i]=analogRead(GUITAR_IN); //read the guitar input
          delay(1);
          
      }
      stopTime = millis(); //stop time in miliseconds
      sampleTime = ((stopTime-startTime)); //the time it takes for all 800 samples is equal to stop time-start time
      for(int k=0; k<660; k++){
          if ((sample[k]>sample[k-1]) & (sample[k]>sample[k+1])) //determines the number of peaks after 800 samples
          {
            peaks++;
          }
      }
      
      period = sampleTime/peaks;   
      frequency = 1000.0/period;
      
      if(frequency >= 77.41 && frequency <= 87.41){  
        hit = 1;
      }
      if((frequency > 102) && (frequency <118)){
        hit = 1;
      }
      if((frequency > 138) && (frequency <154)){
        hit = 1;
      }
      if ((frequency > 238) && (frequency <254)){
        hit = 1;
      }
      if((frequency > 322) && (frequency <338)){
        hit = 1;
      }

      for(int i =0;i<5;i++){
      
        if(minvalue > frequencyArray[i] - frequency){
          minvalue = frequencyArray[i] - frequency;
          closestString = i;
        }
      }
      Serial.print("frequency: ");
      Serial.println(frequency);
      
      break;
    case TransferData: 
        if(hit == 0){
        choice = 1;
        myWork1.write(1);
        
        }
        else if(hit == 1){
        choice = 0;
        Serial.println("HIT");
        myWork2.write(2);
        hit = 0;
        
        }
        break;
    }
}


//
