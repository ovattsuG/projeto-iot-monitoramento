#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// --- Configurações WIFI ---
const char* ssid = "203";
const char* password = "12345678@";
WiFiClient espClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_ClientID = "termometro-teste01";
PubSubClient client(espClient);
const char* topicoTemperatura = "quarto/temperatura";
const char* topicoUmidade = "quarto/umidade";

// --- Configurações do Display ---
#define LARGURA 128
#define ALTURA 64
Adafruit_SSD1306 display(LARGURA, ALTURA, &Wire, -1);

// --- Configurações do DHT ---
#define DHTPIN D3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int umidade;
int temperatura;

void setup() {
  // Serial.begin(115200);
  dht.begin();
  
  configurarDisplay();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();
  medirTemperaturaUmidade();
  mostrarTemperaturaUmidade();
  publicarTemperaturaUmidadeNoTopico();
}


// --- Funçoes Aux --- 

void setup_wifi() {

  delay(10);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Conectando");
  display.display();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }

}

void reconectarMQTT() {
  while (!client.connected()) {
    client.connect(mqtt_ClientID);
  }
}

void configurarDisplay() {
  // Tenta iniciar o display no endereço 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Erro no Display OLED");
    for(;;); // Trava se não achar o display
  }
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();
}

void medirTemperaturaUmidade() {
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    umidade = (int)h;       
    temperatura = (int)t; 
  }

}

void publicarTemperaturaUmidadeNoTopico() {
  client.publish(topicoTemperatura, String(temperatura).c_str(), true);
  client.publish(topicoUmidade, String(umidade).c_str(), true);
}
void mostrarTemperaturaUmidade() {
  
  mostrarMensagemNoDisplay("Temp:", temperatura, "C");
  
  mostrarMensagemNoDisplay("Umid:", umidade, "%");
}

void mostrarMensagemNoDisplay(const char* titulo, int valor, const char* unidade) {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(titulo);
  
  display.setTextSize(5); 
 
  display.setCursor(10, 20); 
  display.print(valor);
  
  display.setTextSize(2);
  display.setCursor(80, 40); // Posiciona a unidade ao lado do número
  display.print(unidade);
  
  display.display();
  
  delay(2000); 
}