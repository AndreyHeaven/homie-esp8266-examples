#include <Homie.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define DHTTYPE DHT22
#define DHTPIN  D2
#define ONE_WIRE_BUS D3
ADC_MODE(ADC_VCC);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DHT dht(DHTPIN, DHTTYPE); // 11 works fine for ESP8266
const int BATT_MIN = 1536;
const int BATT_MAX = 2660;
const int TEMPERATURE_INTERVAL = 600;
const int sleepTimeS = TEMPERATURE_INTERVAL;
unsigned long lastTemperatureSent = 0;

HomieNode temp("temperature","sensor");
HomieNode temp_out("temperature_out","sensor");
HomieNode humidity("humidity","sensor");
HomieNode battery("battery","sensor");

void setupHandler() {
  temp.setProperty("unit").send("c");
  temp_out.setProperty("unit").send("c");
  dht.begin();           // initialize temperature sensor
  sensors.begin();//start ds18b20
}

void loopHandler() {
  if (millis() - lastTemperatureSent >= TEMPERATURE_INTERVAL * 1000UL || lastTemperatureSent == 0) {


    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    float humidity_val = dht.readHumidity();          // Read humidity (percent)
    float temp_val = dht.readTemperature();     // Read temperature as Fahrenheit
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity_val) || isnan(temp_val)) {
      Homie.getLogger() << "Failed to read from DHT sensor! " << endl;
      //return;
    } else {
      Homie.getLogger() << "Temperature: " << temp_val << " °C" << endl;
      Homie.getLogger() << "Humidity: " << humidity_val << " %" << endl;

      temp.setProperty("value").send(String(temp_val));
      humidity.setProperty("value").send(String(humidity_val));
    }

    sensors.requestTemperatures();

    float temp_out_val = sensors.getTempCByIndex(0);
    if (isnan(temp_out_val)) {
      Homie.getLogger() << "Failed to read from DC18B20 sensor! " << endl;
      //return;
    } else {
      Homie.getLogger() << "Temperature out: " << temp_out_val << " °C" << endl;
      temp_out.setProperty("value").send(String(temp_out_val));
    }

    float voltaje=0.00f;
    voltaje = ESP.getVcc();
    voltaje = (voltaje-BATT_MIN)*100/(BATT_MAX-BATT_MIN);
    Homie.getLogger() << "Battery is: " << voltaje << "%"<< endl;
    battery.setProperty("value").send(String(voltaje));
    lastTemperatureSent = millis();
    // if (!isnan(humidity_val) && !isnan(temp_val) && !isnan(temp_out_val)) {
      // Homie.prepareToSleep();
    // }
  }

}
void onHomieEvent(const HomieEvent& event) {
  switch(event.type) {
    //case HomieEventType::MQTT_CONNECTED:
     // Homie.getLogger() << "MQTT connected, preparing for deep sleep..." << endl;
      //Homie.prepareToSleep();
     // break;
    case HomieEventType::READY_TO_SLEEP:
      Homie.getLogger() << "Ready to sleep" << endl;
      ESP.deepSleep(sleepTimeS * 1000000);
      break;
  }
}
void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  Homie_setFirmware("temperature-humidity" , "0.0.3");
  Homie_setBrand("HomeLogic");

  temp.advertise("unit");
  temp.advertise("value");

  temp_out.advertise("unit");
  temp_out.advertise("value");

  humidity.advertise("value");

  battery.advertise("value");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
