#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <DHT.h>

// WiFi
const char* ssid = "OPPO";
const char* password = "n5yyfe5a";

// Servidor Spring Boot
const char* host = "154.12.246.223";
const int port = 8080;

// Endpoints
const char* endpointDHT = "/api/dht11";
const char* endpointYL = "/api/yl69";
const char* endpointRele = "/api/rele";

// Pines sensores
#define DHTPIN 6
#define DHTTYPE DHT11
#define YL69PIN A2
DHT dht(DHTPIN, DHTTYPE);

// Pines de los relés
const int relays[6] = {7, 8, 9, 10, 11, 12};
bool estadoAnterior[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

// Tiempo
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -7 * 3600, 60000); // UTC-7 Sonora
WiFiClient wifi;
HttpClient client = HttpClient(wifi, host, port);

void setup() {
  Serial.begin(115200);
  dht.begin();

  for (int i = 0; i < 6; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], HIGH);
  }

  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ ¡Conectado a WiFi!");

  timeClient.begin();
  timeClient.update();
}

void loop() {
  timeClient.update();
  setTime(timeClient.getEpochTime());

  leerDHT11();
  leerYL69();

  delay(10000); // Cada 10 segundos
}

void leerDHT11() {
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("⚠️ Error al leer DHT11.");
    return;
  }

  String payload = "{";
  payload += "\"sensor\":\"DHT11\",";
  payload += "\"temperatura\":\"" + String(temperatura) + "°C\",";
  payload += "\"humedad\":\"" + String(humedad) + "%\"";
  payload += "}";

  enviarHTTP(endpointDHT, payload);
}

void leerYL69() {
  int humedadAnalog = analogRead(YL69PIN);
  int humedadPorcentaje = map(humedadAnalog, 1023, 0, 0, 100);

  Serial.print("🌱 Humedad del suelo: ");
  Serial.print(humedadPorcentaje);
  Serial.println("%");

  // Enviar lectura del YL-69
  String payload = "{";
  payload += "\"sensor\":\"YL-69\",";
  payload += "\"humedad\":\"" + String(humedadPorcentaje) + "%\"";
  payload += "}";

  enviarHTTP(endpointYL, payload);

  // Control automático de riego
  for (int i = 0; i < 6; i++) {
    bool estadoActual = digitalRead(relays[i]);

    if (humedadPorcentaje < 60 && estadoActual == HIGH) {
      digitalWrite(relays[i], LOW);
      estadoAnterior[i] = LOW;

      String payloadRele = "{";
      payloadRele += "\"id\":" + String(i + 1) + ",";
      payloadRele += "\"encendido\":\"sí\",";
      payloadRele += "\"apagado\":\"no\",";
      payloadRele += "\"fechaHoraEncendido\":\"" + getFechaHora() + "\"";
      payloadRele += "}";

      enviarHTTP(endpointRele, payloadRele);
    }
    else if (humedadPorcentaje >= 60 && estadoActual == LOW) {
      digitalWrite(relays[i], HIGH);
      estadoAnterior[i] = HIGH;

      String payloadRele = "{";
      payloadRele += "\"id\":" + String(i + 1) + ",";
      payloadRele += "\"encendido\":\"no\",";
      payloadRele += "\"apagado\":\"sí\",";
      payloadRele += "\"fechaHoraApagado\":\"" + getFechaHora() + "\"";
      payloadRele += "}";

      enviarHTTP(endpointRele, payloadRele);
    }
  }
}

String getFechaHora() {
  char buffer[20];
  sprintf(buffer, "%02d/%02d/%04d %02d:%02d", day(), month(), year(), hour(), minute());
  return String(buffer);
}

void enviarHTTP(const char* endpoint, String payload) {
  Serial.println("📤 Enviando a " + String(endpoint) + ": " + payload);

  client.beginRequest();
  client.post(endpoint);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", payload.length());
  client.beginBody();
  client.print(payload);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("📥 Código HTTP: ");
  Serial.println(statusCode);
  Serial.print("📨 Respuesta: ");
  Serial.println(response);
}

