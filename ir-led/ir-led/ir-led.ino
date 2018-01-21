#include <Homie.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>

uint16_t SEND_PIN = D6;
IRsend irsend(SEND_PIN); 
uint16_t RECV_PIN = 4;
decode_results results;

IRrecv irrecv(RECV_PIN);
HomieNode irFishNode("led", "light");
HomieNode irNode("ir", "ir");
HomieNode irVacuumNode("hombot", "vacuum");

void setupHandler() {
  irsend.begin();
  irrecv.enableIRIn();  // Start the receiver

}
void loopHandler() {
    if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    unsigned long long1 = (unsigned long)((results.value & 0xFFFF0000) >> 16 );
  unsigned long long2 = (unsigned long)((results.value & 0x0000FFFF));

  String hex = String(long1, HEX) + String(long2, HEX); // six octets
//    String val = String((int) results.value, HEX);
    Homie.getLogger() << "IR: " << hex << endl;
    irVacuumNode.setProperty("ir").send(hex);
//    serialPrintUint64(results.value, HEX);
//    Serial.println("");
    irrecv.resume();  // Receive the next value
  }

}

bool irHandler(const HomieRange& range, const String& value) {
  if (value == "on" || value == "ON"){
    irsend.sendNEC(0xF7C03F);
  } else {
    irsend.sendNEC(0xF740BF);
  }
  irFishNode.setProperty("on").send(value); // Update the state of the light

  return true;
}
bool irVacuumHandler(const HomieRange& range, const String& value) {
  if (value == "turn_on"){
    irsend.sendNEC(0xF7C03F);
  }
  if (value == "stop" || value == "turn_off"){
    irsend.sendNEC(0xF7C03F);
  }
  if (value == "return_to_base"){
    irsend.sendNEC(0xF7C03F);
  }
   
  irVacuumNode.setProperty("command").send(value);

  return true;
}
void setup() {
  Homie_setFirmware("ir-server" , "0.0.1");

  Homie_setBrand("HomeLogic");
  Serial.begin(115200);
  Serial << endl << endl;

  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  irFishNode.advertise("on").settable(irHandler);
  irNode.advertise("ir");
  irVacuumNode.advertise("command").settable(irVacuumHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();

}
