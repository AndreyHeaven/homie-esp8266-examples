#include <Homie.h>
#define LDR A0
//#define RED D8
//#define GREEN D6
//#define BLUE D7
#define BUTTON 4
const int sleepTimeS = 5;
//HomieNode node("sample","ESH:Switch");
HomieNode buttonNode("button","ESH:Switch");
//ADC_MODE(ADC_VCC);
void setupHandler() {
//  pinMode(BLUE, OUTPUT);
//  digitalWrite(BLUE, LOW);
//  pinMode(GREEN, OUTPUT);
//  digitalWrite(GREEN, LOW);
//  pinMode(RED, OUTPUT);
//  digitalWrite(RED, LOW);
}

void loopHandler() {
  //Homie.getLogger() << ESP.getVcc() << endl;
//  digitalWrite(BLUE, HIGH);
//  delay(2000);
//  digitalWrite(BLUE, LOW);
//  delay(2000);
//  Homie.prepareToSleep();
}

void onHomieEvent(const HomieEvent& event) {
  switch(event.type) {
    //case HomieEventType::MQTT_CONNECTED:
     // Homie.getLogger() << "MQTT connected, preparing for deep sleep..." << endl;
      //Homie.prepareToSleep();
     // break;
    case HomieEventType::READY_TO_SLEEP:
      Homie.getLogger() << "Ready to sleep" << endl;
      ESP.deepSleep(sleepTimeS * 1000000, WAKE_RF_DEFAULT);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello");
  Homie_setFirmware("basic" , "0.0.3");

  Homie_setBrand("HomeLogic");

  // Homie.setLedPin(12, HIGH);

  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  buttonNode.advertise("value");

  Homie.setup();
}

void loop() {
  Homie.loop();

  // float voltaje=0.00f;
  // voltaje = ESP.getVcc();
  // Serial.println(voltaje/1024.00f);
}
