#include <Arduino.h>
#include "config.h"
#include "rgb-random.h"
unsigned long previousMillis = 0;
const long interval = 1000;
int rainbow[7][3] = {{255,0,0}
                  ,{255,127,0}
                  ,{255,255,0}
                  ,{0,255,0}
                  ,{0,0,255}
                  ,{75,0,130}
                  ,{139,0,255}};
void random_setup() {
 pinMode(redPIN, OUTPUT);
 analogWrite(redPIN,LOW);
 pinMode(bluePIN, OUTPUT);
 analogWrite(bluePIN,LOW);
 pinMode(greenPIN, OUTPUT);
 analogWrite(greenPIN,LOW);

 randomSeed(analogRead(0));
}


void random_loop(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int p = random(7);
    int red = rainbow[p][0];
    int green = rainbow[p][1];
    int blue = rainbow[p][2];
    analogWrite(redPIN,red);
    analogWrite(greenPIN,green);
    analogWrite(bluePIN,blue);

  }

}
