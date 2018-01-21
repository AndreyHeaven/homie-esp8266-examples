#include <Homie.h>
#include <Button.h>
#define LDR A0
#define RED 15
#define GREEN 12
#define BLUE 13
#define BUTTON 4
const unsigned char NUMBER_OF_LED = 3;
const unsigned char LED_PINS[NUMBER_OF_LED] = { RED, GREEN, BLUE};
const int INTERVAL = 1;
Button button(BUTTON);

//HomieNode node("sample","ESH:Switch");
unsigned long lastLightSent = 0;

HomieNode lightNode("light","ESH:Number");
HomieNode buttonNode("button","ESH:Switch");
HomieNode rgbNode("rgb", "ESH:RGB");

void setupHandler() {

}

void loopHandler() {
  if (button.toggled()) {
		if (button.read() == Button::PRESSED)
			buttonNode.setProperty("value").send("ON");
		else
			buttonNode.setProperty("value").send("OFF");
	}
  if (millis() - lastLightSent >= INTERVAL * 1000UL || lastLightSent == 0) {
    lightNode.setProperty("value").send(String(analogRead(LDR)));
    lastLightSent = millis();
  }
}

bool rgbLedHandler(const HomieRange& range, const String& value) {
  if (!range.isRange) return false;  // if it's not a range

  if (range.index < 1 || range.index > NUMBER_OF_LED) return false;  // if it's not a valid range

  //if (value != "on" && value != "off") return false;  // if the value is not valid

  if (value == "on" || value == "off"){
    bool on = (value == "on");
    analogWrite(LED_PINS[range.index - 1], on ? PWMRANGE : 0);
  } else {
    long v = value.toInt();
    if (v>PWMRANGE) {v = PWMRANGE;}
    if (v<0) {v = 0;}
    analogWrite(LED_PINS[range.index - 1], v);
  }
  rgbNode.setProperty("led").setRange(range).send(value);  // Update the state of the led
  Homie.getLogger() << "Led " << range.index << " is " << value << endl;

  return true;
}


void setup() {
  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware("basic-sample" , "0.0.2");

  Homie_setBrand("HomeLogic");

  //Homie.setLedPin(GREEN, HIGH);

  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  buttonNode.advertise("value");
  lightNode.advertise("value");

  for (int i = 0; i < NUMBER_OF_LED; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    // digitalWrite(LED_PINS[i], LOW);
    analogWrite(LED_PINS[i], 0);
  }
  rgbNode.advertiseRange("led", 1, NUMBER_OF_LED).settable(rgbLedHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
