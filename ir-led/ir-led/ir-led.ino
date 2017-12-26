#include <Homie.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

IRsend irsend(D6); 
HomieNode irNode("led", "light");

void setupHandler() {
  irsend.begin();
}
void loopHandler() {
}

bool irHandler(const HomieRange& range, const String& value) {
  if (value == "on" || value == "ON"){
    irsend.sendNEC(0xF7C03F);
  } else {
    irsend.sendNEC(0xF740BF);
  }
  irNode.setProperty("on").send(value); // Update the state of the light

  return true;
}
void setup() {
  Homie_setFirmware("ir-server" , "0.0.1");

  Homie_setBrand("HomeLogic");
  Serial.begin(115200);
  Serial << endl << endl;

  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  irNode.advertise("on").settable(irHandler);

  Homie.setup();
}

void loop() {
  Homie.loop();

}
