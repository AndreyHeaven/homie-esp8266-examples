#include <Homie.h>

const int PIN_RELAY1 = D0;
const int PIN_RELAY2 = D1;
const int PIN_BUTTON = D2;
const int PIN_LED = D6;
HomieNode light1("light1", "light");
HomieNode light2("light2", "light");

bool light1OnHandler(const HomieRange& range, const String& value) {
  if (value == "on" || value == "ON") {
    digitalWrite(PIN_RELAY1, HIGH);
    Serial.println("Light1 is on");
  } else if (value == "off" || value == "OFF") {
    digitalWrite(PIN_RELAY1, LOW);
    Serial.println("Light1 is off");
  } else {
    return false;
  }
  light1.setProperty("on").send(value); // Update the state of the light

  return true;
}
bool light2OnHandler(const HomieRange& range, const String& value) {
  if (value == "on" || value == "ON") {
    digitalWrite(PIN_RELAY2, HIGH);
    Serial.println("Light2 is on");
  } else if (value == "off" || value == "OFF") {
    digitalWrite(PIN_RELAY2, LOW);
    Serial.println("Light2 is off");
  } else {
    return false;
  }
  light2.setProperty("on").send(value); // Update the state of the light

  return true;
}

void setupHandler() {
}

void loopHandler() {

}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY1, OUTPUT);
  digitalWrite(PIN_RELAY1, LOW);
  pinMode(PIN_RELAY2, OUTPUT);
  digitalWrite(PIN_RELAY2, LOW);

  Homie_setFirmware("awesome-relay-2" ,"1.0.0");
  Homie_setBrand("HomeLogic");
  
  light1.advertise("on").settable(light1OnHandler);
  light2.advertise("on").settable(light2OnHandler);
  
  Homie.setLedPin(PIN_LED, HIGH).setResetTrigger(PIN_BUTTON, LOW, 5000);
  
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
