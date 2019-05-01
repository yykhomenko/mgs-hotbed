#ifndef MetricSender_h
#define MetricSender_h

#include "infras/mqtt/MqttClient.h"

class MetricSender {

private:
  MqttClient *client;
  u16 period;
  std::function<std::vector<String>(void)> states;
  u64 lastSentTime = 0;

public:
  MetricSender(MqttClient *client, u16 period,
      const std::function<std::vector<String>(void)> &metrics) {
    this->client = client;
    this->period = period;
    this->states = metrics;
  }

  void loop() {
    const long now = millis();

    if (now - lastSentTime >= period) {

      lastSentTime = now;

      if (client->enabled())
        for (auto &s : states())
          client->publish("states", s);
    }
  }
};

#endif