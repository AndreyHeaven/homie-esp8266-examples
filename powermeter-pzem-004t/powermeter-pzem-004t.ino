#include <Homie.h>
#include <PZEM004T.h>
#include "RunningMedian.h"

HomieNode meter("powermeter", "powermeter");

PZEM004T pzem(4, 5); // RX,TX (D2, D1) on NodeMCU
IPAddress ip(192, 168, 1, 1);
const int READ_INTERVAL = 1;
const int UPDATE_INTERVAL = 60;
unsigned long lastRead = 0;
unsigned long lastSent = 0;
bool pzemrdy = false;

RunningMedian samplesV = RunningMedian(UPDATE_INTERVAL / READ_INTERVAL);
RunningMedian samplesI = RunningMedian(UPDATE_INTERVAL / READ_INTERVAL);
RunningMedian samplesP = RunningMedian(UPDATE_INTERVAL / READ_INTERVAL);
RunningMedian samplesE = RunningMedian(UPDATE_INTERVAL / READ_INTERVAL);

void setupHandler() {
  while (!pzemrdy) {
    //    Serial.println("Connecting to PZEM...");
    Homie.getLogger() << "Connecting to PZEM..." << endl;
    pzemrdy = pzem.setAddress(ip);
    delay(1000);
  }
}

void loopHandler() {
  if (millis() - lastRead >= READ_INTERVAL * 1000UL || lastRead == 0) {

    float v = pzem.voltage(ip);
    if (v >= 0.0) {
      Homie.getLogger() << v << endl;
      samplesV.add(v);
    }
    float i = pzem.current(ip);
    if (i >= 0.0) {
      Homie.getLogger() << i << "A" << endl;
      samplesI.add(i);
    }

    float p = pzem.power(ip);
    if (p >= 0.0) {
      Homie.getLogger() << p << "W" << endl;
      samplesP.add(p);
      
    }

    float e = pzem.energy(ip);
    if (e >= 0.0) {
      Homie.getLogger() << e << "Wh" << endl;
      samplesE.add(e);
    }

    lastRead = millis();
  }
  if (millis() - lastSent >= UPDATE_INTERVAL * 1000UL || lastSent == 0) {
    meter.setProperty("voltage").send(String(samplesV.getMedian()));
    samplesV.clear();
    meter.setProperty("current").send(String(samplesI.getMedian()));
    samplesI.clear();
    meter.setProperty("power").send(String(samplesP.getMedian()));
    samplesP.clear();
    meter.setProperty("energy").send(String(samplesE.getMedian()));
    samplesE.clear();
    
    lastSent = millis();
  }
}
void setup() {
  Serial.begin(115200);
  Homie_setFirmware("powermeter" , "0.0.1");
  Homie_setBrand("HomeLogic");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  meter.advertise("voltage");
  meter.advertise("current");
  meter.advertise("power");
  meter.advertise("energy");

  Homie.setup();
}

void loop() {
  Homie.loop();
}
