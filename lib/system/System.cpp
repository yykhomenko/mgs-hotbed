#include "System.h"

#include <WiFiUdp.h>
#include <infras/ntp/NtpClient.h>
#include <pixeltypes.h>

#include "controls/metricSender/MetricSender.h"
#include "actuators/humidifier/Humidifier.h"
#include "controls/gigrostat/Gigrostat.h"

WiFiUDP udp;
NtpClient ntpClient(udp);

System::System(const Conf &conf) {
  this->conf = conf;
}

void System::setup() {

  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  offLed();

  wifi = new WifiDevice(conf.ssid, conf.password, [this](bool in) {
    Serial.print(".");
    if (in) onLed();
    else offLed();
  });

  ntpClient.begin();

  ota = new Ota(conf.system, conf.service);

  const String queuePrefix = conf.group + "/" + conf.system + "/" + conf.service;

  mqttClient = new MqttClient(
      "35.241.228.120", 1883, "", "",
      queuePrefix,
      [&] (char* topic, u8* payload, u32 length) {

    Serial.print("[MqttClient] Message arrived in topic: "); Serial.print(topic);

    payload[length] = 0;
    const String message = String((char*) payload);
    Serial.println(", message:" + message);
  });

  metricSender = new MetricSender(mqttClient, 2000, queuePrefix, [this] {

    onLed();

    std::vector<String> metrics;

    if (dhtSensor) metrics.push_back(dhtSensor->metrics());
    if (humidifier) metrics.push_back(humidifier->metrics());
    if (ventilation) metrics.push_back(ventilation->metrics());

    offLed();

    return metrics;
  });
}

void System::loop() {

  ntpClient.update();

  if (ota)          ota->loop();
  if (mqttClient)   mqttClient->loop();
  if (metricSender) metricSender->loop();
  if (gigrostat)    gigrostat->loop();
}

void System::setupPin(u8 pin, u8 mode) {
  pinMode(pin, mode);
}

void System::setupDht(u8 pin, u8 type) {
  dhtSensor = new DhtSensor(pin, type);
}

void System::setupHumidifier(u8 pin, u8 statePin) {
  humidifier = new Humidifier(pin, statePin);
}

void System::setupLighting(u8 pin, u16 ledNum) {
  lighting = new Lighting(pin, ledNum);
  lighting->setup(CRGB::White, 7000, 4);
}

void System::setupVentilation(u8 pin) {
  ventilation = new Ventilation(pin);
}

void System::setupGigrostat(real32 min, real32 max) {
  gigrostat = new Gigrostat(dhtSensor, humidifier, ventilation);
  gigrostat->setup(min, max);
}

void System::onPin(u8 pin) {
  digitalWrite(pin, HIGH);
}

void System::offPin(u8 pin) {
  digitalWrite(pin, LOW);
}

void System::onLed() {
  offPin(LED_BUILTIN);
}

void System::offLed() {
  onPin(LED_BUILTIN);
}

void System::withBlink(const std::function<void(void)> &body) {
  onLed();
  body();
  offLed();
}