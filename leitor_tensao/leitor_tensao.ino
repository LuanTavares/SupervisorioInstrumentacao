#include <stdio.h>
#include <stdlib.h>

int analogPin = 0;
int val = 0;
//float unitario = 0.0048828125; //1/1024
float res = 0;

void setup()
{
  Serial.begin(115200);          //  setup serial  
  pinMode(50, OUTPUT); 
}

void loop()
{
   val = analogRead(analogPin);
   
   digitalWrite(50, HIGH);
   //if (val > 0)
   //{

      
      Serial.print("[");
      Serial.print(val);
      Serial.print("]"); 
   //}
   
   delay(5); //20ms = 1/50Hz
   digitalWrite(50, LOW);
}

