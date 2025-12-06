#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <nicolascorneli-project-1_inferencing.h>

// LEDs
#define LED_UP_DOWN 32
#define LED_LEFT_RIGHT 33
#define LED_FRONT_BACK 35

// WiFi + MQTT
const char* ssid = "AMF";
const char* password = "amf@2025";
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor
MPU6050 mpu(0x68);

// Edge Impulse
const float SAMPLE_RATE_HZ = EI_CLASSIFIER_FREQUENCY;
const unsigned long SAMPLE_INTERVAL_MS = 1000.0f / SAMPLE_RATE_HZ;
unsigned long last_sample = 0;

static float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
static int buffer_index = 0;

void mqtt_reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("TinyML_ESP32")) {
      Serial.println("MQTT conectado!");
    } else {
      Serial.print(".");
      delay(1000);
    }
  }
}

void wifi_connect() {
  Serial.print("Conectando ao WiFi ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi conectado!");
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  // LEDs
  pinMode(LED_UP_DOWN, OUTPUT);
  pinMode(LED_LEFT_RIGHT, OUTPUT);
  pinMode(LED_FRONT_BACK, OUTPUT);

  // WiFi + MQTT
  wifi_connect();
  client.setServer(mqtt_server, mqtt_port);

  // Sensor
  Wire.begin(23, 22);
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("ERRO: MPU6050 não detectado!");
    while (1);
  }

  Serial.println("🧠 IA + MQTT pronta!");
}

void loop() {
  if (!client.connected()) mqtt_reconnect();
  client.loop();

  if ((millis() - last_sample) < SAMPLE_INTERVAL_MS) return;
  last_sample = millis();

  int16_t ax_raw, ay_raw, az_raw;
  mpu.getAcceleration(&ax_raw, &ay_raw, &az_raw);

  const float ACCEL_SENS = 16384.0f;
  float ax = (ax_raw / ACCEL_SENS) * 9.80665f;
  float ay = (ay_raw / ACCEL_SENS) * 9.80665f;
  float az = (az_raw / ACCEL_SENS) * 9.80665f;

  buffer[buffer_index++] = ax;
  buffer[buffer_index++] = ay;
  buffer[buffer_index++] = az;

  if (buffer_index < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) return;
  buffer_index = 0;

  signal_t signal;
  numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
  ei_impulse_result_t result;
  run_classifier(&signal, &result, false);

  Serial.println("----- IA -----");

  // Apaga os LEDs
  digitalWrite(LED_UP_DOWN, LOW);
  digitalWrite(LED_LEFT_RIGHT, LOW);
  digitalWrite(LED_FRONT_BACK, LOW);

  String movimento = "Nenhum";

  if (result.classification[0].value > 0.80f) {
    movimento = "CIMA_BAIXO";
    digitalWrite(LED_UP_DOWN, HIGH);
  }
  else if (result.classification[1].value > 0.80f) {
    movimento = "DIREITA_ESQUERDA";
    digitalWrite(LED_LEFT_RIGHT, HIGH);
  }
  else if (result.classification[2].value > 0.80f) {
    movimento = "FRENTE_TRAS";
    digitalWrite(LED_FRONT_BACK, HIGH);
  }

  Serial.println("Movimento: " + movimento);
  client.publish("tinyml/movimento", movimento.c_str());
}
