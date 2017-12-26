#include <Arduino.h>

#include <Homie.h>
#include "config.h"
#include "rgb-rainbow.h"
#include "rgb-random.h"


// note
// TX GPIO2 @Serial1 (Serial ONE)
// RX GPIO3 @Serial

#ifdef redPIN
HomieNode rgbNode("rgb", "light");
boolean m_rgb_state = false;
uint8_t m_rgb_brightness = 100;
uint8_t m_rgb_red = 255;
uint8_t m_rgb_green = 255;
uint8_t m_rgb_blue = 255;
String m_rgb_effect = "";
#endif

#ifdef w1PIN
HomieNode wNode("w", "light");
boolean m_w_state = false;
uint8_t m_w_brightness = 100;
#endif

#ifdef w2PIN
HomieNode wwNode("ww", "light");
boolean m_ww_state = false;
uint8_t m_ww_brightness = 100;
#endif

void setupHandler() {
  #ifdef greenPIN
  pinMode(redPIN, OUTPUT);
  analogWrite(redPIN, 0);

  pinMode(greenPIN, OUTPUT);
  analogWrite(greenPIN, 0);

  pinMode(bluePIN, OUTPUT);
  analogWrite(bluePIN, 0);


  rgbNode.setProperty("on").send("OFF");
  rgbNode.setProperty("effect").send(m_rgb_effect);
  #endif
  #ifdef w1PIN
  pinMode(w1PIN, OUTPUT);
  analogWrite(w1PIN, 0);
  wNode.setProperty("on").send("OFF");
  #endif
  #ifdef w2PIN
  pinMode(w2PIN, OUTPUT);
  analogWrite(w2PIN, 0);
  wwNode.setProperty("on").send("OFF");
  #endif
}

void loopHandler() {
  if (m_rgb_state) {
    if (m_rgb_effect == "rainbow"){
      rainbow_loop();
    }
    if (m_rgb_effect == "random"){
      random_loop();
    }
  }
}

int normalize(int raw, int max) {
  if (raw > max) {
    return max;
  } else if (raw < 0) {
    return 0;
  } else {
    return raw;
  }
}
bool rgbLedOnHandler(const HomieRange& range, const String& value) {
  m_rgb_state = (value == "on" || value == "ON");
  setColor();
  if (!m_rgb_state) {
    m_rgb_effect = "";
    rgbNode.setProperty("effect").send(m_rgb_effect);
  }
  //rgbNode.setProperty("on").send(value);  // Update the state of the led
  return true;
}
bool rgbLedHandler(const HomieRange& range, const String& value) {
  // get the position of the first and second commas
  uint8_t firstIndex = value.indexOf(',');
  uint8_t lastIndex = value.lastIndexOf(',');

  m_rgb_red = normalize(value.substring(0, firstIndex).toInt(), 255);
  m_rgb_green = normalize(value.substring(firstIndex + 1, lastIndex).toInt(), 255);
  m_rgb_blue = normalize(value.substring(lastIndex + 1).toInt(), 255);
  setColor();
  rgbNode.setProperty("effect").send("");
  //rgbNode.setProperty("rgb").send(value);  // Update the state of the led
  Homie.getLogger() << "rgb is " << m_rgb_red << "," << m_rgb_green << "," << m_rgb_blue << endl;
  m_rgb_effect = "";
  return true;
}

bool rgbLedBrightnessHandler(const HomieRange& range, const String& value) {
  m_rgb_brightness = normalize(value.toInt(), 100);
  setColor();
  //rgbNode.setProperty("brightness").send(value);  // Update the state of the led
  Homie.getLogger() << "brightness is " << m_rgb_brightness << endl;

  return true;
}

bool rgbLedActioHandler(const HomieRange& range, const String& value) {

  m_rgb_effect = value;
  if (m_rgb_effect == "rainbow"){
    rainbow_setup();
  }
  if (m_rgb_effect == "random"){
    random_setup();
  }
  rgbNode.setProperty("effect").send(m_rgb_effect);
}


// function called to adapt the brightness and the color of the led
void setColor() {
  if (m_rgb_state) {
    // convert the brightness in % (0-100%) into a digital value (0-255)
    int brightness = map(m_rgb_brightness, 0, 100, 0, 255);
    Homie.getLogger() << "set brightness pwm is " << brightness << endl;
    int c = map(m_rgb_red, 0, 255, 0, brightness);
    analogWrite(redPIN, c);
    Homie.getLogger() << "set red pwm is " << c << endl;
    c = map(m_rgb_green, 0, 255, 0, brightness);
    analogWrite(greenPIN, c);
    Homie.getLogger() << "set green pwm is " << c << endl;
    c = map(m_rgb_blue, 0, 255, 0, brightness);
    analogWrite(bluePIN, c);
    Homie.getLogger() << "set blue pwm is " << c << endl;
  } else {
    analogWrite(redPIN, 0);
    analogWrite(greenPIN, 0);
    analogWrite(bluePIN, 0);
  }
  rgbNode.setProperty("brightness").send(String(m_rgb_brightness));
  rgbNode.setProperty("on").send(m_rgb_state ? "ON" : "OFF");
  String s = String(m_rgb_red);
  s += ",";
  s += m_rgb_green;
  s += ",";
  s += m_rgb_blue;
  rgbNode.setProperty("rgb").send(s);
}

#ifdef w1PIN
void setW() {
  if (m_w_state) {
    analogWrite(w1PIN, map(m_w_brightness, 0, 100, 0, 255));
  } else {
    analogWrite(w1PIN, 0);
  }
}
bool wLedHandler(const HomieRange& range, const String& value) {

  if (value == "on" || value == "off" || value == "ON" || value == "OFF") {
    m_w_state = (value == "on" || value == "ON");
    setW();
    wNode.setProperty("on").send(value);  // Update the state of the led
    wNode.setProperty("brightness").send(String(m_w_brightness));
  } else {
    m_w_brightness = normalize(value.toInt(), 100);
    setW();
    wNode.setProperty("brightness").send(value);  // Update the state of the led
  }
  Homie.getLogger() << "Led W is " << value << endl;

  return true;
}

#endif

#ifdef w2PIN
void setWW() {
  if (m_ww_state) {
    analogWrite(w2PIN, map(m_ww_brightness, 0, 100, 0, 255));
  } else {
    analogWrite(w2PIN, 0);
  }
}

bool wwLedHandler(const HomieRange& range, const String& value) {

  if (value == "on" || value == "off" || value == "ON" || value == "OFF") {
    m_ww_state = (value == "on" || value == "ON");
    setWW();
    wwNode.setProperty("on").send(value);  // Update the state of the led
    wwNode.setProperty("brightness").send(String(m_ww_brightness));
  } else {
    m_ww_brightness = normalize(value.toInt(), 100);
    setWW();
    wwNode.setProperty("brightness").send(value);  // Update the state of the led
  }
  Homie.getLogger() << "Led W is " << value << endl;

  return true;

}
#endif



void setup() {
  Homie_setFirmware("rgbww-strip" , "0.0.4");

  Homie_setBrand("HomeLogic");

  // Serial.begin(115200);
  // Serial << endl << endl;
  Serial1.begin(115200);
  delay(10);
  Serial1.println();
  Serial1.println();
  Homie.setLoggingPrinter(&Serial1);

  #ifdef POWER_LED_PIN
  pinMode(POWER_LED_PIN, OUTPUT);
  digitalWrite(POWER_LED_PIN, LOW);
  #endif
  #ifdef OPERATION_LED_PIN
  Homie.setLedPin(OPERATION_LED_PIN, LOW);
  #endif

  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);


  #ifdef greenPIN
  pinMode(redPIN, OUTPUT);
  analogWrite(redPIN, LOW);
  pinMode(bluePIN, OUTPUT);
  analogWrite(bluePIN, LOW);
  pinMode(greenPIN, OUTPUT);
  analogWrite(greenPIN, LOW);

  rgbNode.advertise("on").settable(rgbLedOnHandler);
  rgbNode.advertise("rgb").settable(rgbLedHandler);
  rgbNode.advertise("brightness").settable(rgbLedBrightnessHandler);
  rgbNode.advertise("effect").settable(rgbLedActioHandler);
  #endif

  #ifdef w1PIN
  pinMode(w1PIN, OUTPUT);
  analogWrite(w1PIN, 0);

  wNode.advertise("on").settable(wLedHandler);
  wNode.advertise("brightness").settable(wLedHandler);
  #endif

  #ifdef w2PIN
  pinMode(w2PIN, OUTPUT);
  analogWrite(w2PIN, 0);

  wwNode.advertise("on").settable(wwLedHandler);
  wwNode.advertise("brightness").settable(wwLedHandler);
  #endif



  Homie.setup();
}

void loop() {
  Homie.loop();

}
