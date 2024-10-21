#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <math.h>
// maun
// Configurações da rede WiFi
#define WIFI_SSID "L"
#define WIFI_PASSWORD "llm404050501212"

// URL do Google Script
#define GOOGLE_SCRIPT_ID "https://script.google.com/macros/s/AKfycbxRN4KXPWp38ZcplGD4x0xZtbV2pWrae_ClceeldZ_kaSwCWfL1JjtVmqxA7Qls_mChog/exec"

// Definição dos pinos dos sensores DS18B20
#define ONE_WIRE_BUS_1 19
#define ONE_WIRE_BUS_2 18

// Instâncias do sensor de temperatura
OneWire oneWire1(ONE_WIRE_BUS_1);
OneWire oneWire2(ONE_WIRE_BUS_2);

DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors2(&oneWire2);

// Instância do acelerômetro
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi!");

  if (!accel.begin()) {
    Serial.println("ADXL345 não detectado");
    while (1);
  }

  // Iniciar os sensores de temperatura
  sensors1.begin();
  sensors2.begin();
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;
  float magnitude = sqrt(x * x + y * y + z * z);

  // Solicitar temperatura de cada sensor
  sensors1.requestTemperatures();
  float temperature1 = sensors1.getTempCByIndex(0);

  sensors2.requestTemperatures();
  float temperature2 = sensors2.getTempCByIndex(0);

  Serial.print("Magnitude: ");
  Serial.println(magnitude);
  Serial.print("Componente X: ");
  Serial.println(x);
  Serial.print("Componente Y: ");
  Serial.println(y);
  Serial.print("Componente Z: ");
  Serial.println(z);
  Serial.print("Temperatura 1: ");
  Serial.println(temperature1);
  Serial.print("Temperatura 2: ");
  Serial.println(temperature2);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(GOOGLE_SCRIPT_ID);
    http.addHeader("Content-Type", "application/json");

    // Enviar dados das temperaturas e da magnitude
    String jsonPayload = "{\"magnitude\":" + String(magnitude) + ", \"x\":" + String(x) + ", \"y\":" + String(y) + ", \"z\":" + String(z) +
                         ", \"temperature1\":" + String(temperature1) +
                         ", \"temperature2\":" + String(temperature2) + "}";
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro no envio HTTP: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Desconectado do WiFi");
  }

  delay(1000);
}
