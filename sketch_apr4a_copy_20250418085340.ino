#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(D7, D8);
 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int trigPin = D6;
const int echoPin = D5;
const int led = D7;
long duration;
int distance;
float level;
const char *ssid = "Sravya";
const char *password = "Abcd1234";
void send_event(const char *event);
const char *host = "maker.ifttt.com";
const char *privateKey = "hUAAAz0AVvc6-NW1UmqWXXv6VQWmpiGFxx3sV5rnaM9";
WiFiServer server(80);
 
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Smart ");
  lcd.setCursor(0, 1);
  lcd.print("      Dustbin");
  delay(3000);
  lcd.clear();
  Serial.print("Connecting to Wifi Network");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi.");
  Serial.println("IP address is : ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("Server started");
}
 
void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0340 / 2;
  Serial.println("Distance");
  Serial.println(distance);
  level = ((25 - distance) / 25.0) * 100;  //25
  Serial.println("level");
  Serial.println(level);
  lcd.setCursor(0, 0);
  lcd.print(" Smart Dustbin");
  lcd.setCursor(0, 1);
  lcd.print(" Level");
  lcd.setCursor(8, 1);
  lcd.print(level);
  digitalWrite(led, LOW);
  delay(200);
  WiFiClient client = server.available();
  if (level >= 80) {
    SendMessage();
    digitalWrite(led, HIGH);
    send_event("jar_event");
  }
}
void send_event(const char *event) {
  Serial.print("Connecting to ");
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  // We now create a URI for the request
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      // No data yet, wait a bit
      delay(50);
    };
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}
void SendMessage() {
  Serial.println("Dustbin Full");
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+9188305848xx\"\r");
  delay(1000);
  mySerial.println("Dustbin1 Is Full Plz Remove Dustbin  https://www.google.com/maps?q ");
  delay(100);
  mySerial.println((char)26);
  delay(1000);
}