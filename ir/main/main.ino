#include <Homie.h>
#define IR_LED 12
#define IR_RECV 4

#include <IRremoteESP8266.h>

IRsend irsend(IR_LED); //an IR led is connected to GPIO pin 4
IRrecv irrecv(IR_RECV);

decode_results results;

//HomieNode node("sample","ESH:Switch");
HomieNode irSendNode("irRemote","switch");
HomieNode irRecvNode("irReciver","sensor");

void setupHandler() {
  irsend.begin();
  irrecv.enableIRIn(); // Start the receiver

}

void loopHandler() {
  if (irrecv.decode(&results)) {
    Homie.getLogger().println(results.value, HEX);
    irRecvNode.setProperty("command").send(String(results.value));
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}

bool commandHandler(const HomieRange& range, const String& value) {
  if (value == "start"){
    // irsend.sendRaw()
    // irsend.send(0x00FFE01F, 36);
  }
  irSendNode.setProperty("command").send(value);  // Update the state of the led
  return true;
}

void setup() {
  Serial.begin(115200);
  Homie_setFirmware("ir-lg-hom-bot-with-recv" , "0.0.1");

  Homie_setBrand("HomeLogic");

  // Homie.setLedPin(12, HIGH);

  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  irSendNode.advertise("command").settable(commandHandler);;
  irRecvNode.advertise("command");

  Homie.setup();
}

void loop() {
  Homie.loop();
}
