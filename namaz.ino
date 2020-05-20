#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
#include <NTPClient.h>

#include <WiFiUdp.h>
#include <ArduinoJson.h>
ESP8266WiFiMulti WiFiMulti;
WiFiUDP ntpUDP;
#define SABAH D4 
#define OGLE D5
#define IKINDI D6 
#define AKSAM D7
#define YATSI D8 
const long utcOffsetInSeconds = 3600;
String ssid = "@ONUR@"; 
String password = "2ee766af";
NTPClient tc(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
pinMode(SABAH, OUTPUT);   
pinMode(OGLE, OUTPUT);   
pinMode(IKINDI, OUTPUT);   
pinMode(AKSAM, OUTPUT);   
pinMode(YATSI, OUTPUT);   
digitalWrite(SABAH, LOW);
digitalWrite(OGLE, LOW);
digitalWrite(IKINDI, LOW);
digitalWrite(AKSAM, LOW);
digitalWrite(YATSI, LOW);

connectWifi();


  //


  tc.begin();

}

void loop() {
  // wait for WiFi connection
  if (WiFi.status() == WL_CONNECTED) { 

  tc.update();

  String gun = String(tc.getDate());
  gun = (tc.getDate() < 10) ? String("0")+gun : gun;
  String ay = String(tc.getMonth());
  ay = (tc.getMonth() < 10) ? String("0")+ay : ay;
  String yil = String(tc.getYear());
 String server = "http://hocaokudumu.com/namazsaati?Ulke=TURKIYE&Sehir=ISTANBUL&Tarih="+gun+"."+ay+"."+yil+"&format=json";
 //const char* server = "http://hocaokudumu.com/namazsaati?Ulke=TURKIYE&Sehir=ISTANBUL&Tarih=14.05.2020&format=json";
  Serial.print(server);
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, server)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          //BURDAN
            int saat = tc.getHours() + 2; //YAZ SAATI
            int dakika = tc.getMinutes();
            const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 370;
            DynamicJsonBuffer jsonBuffer(bufferSize);
            JsonObject& root = jsonBuffer.parseObject(http.getString());
            // Parameters
            const char s[2] = ":";
            char gunes[6];
            strcpy(gunes,root["Gunes"]);
            char oglen[6];
            strcpy(oglen,root["Oglen"]);
            char ikindi[6];
            strcpy(ikindi,root["Ikindi"]);
            char aksam[6];
            strcpy(aksam,root["Aksam"]);
            char yatsi[6];
            strcpy(yatsi,root["Yatsi"]);
            Serial.printf("%s",strtok(oglen,":"));
            char *token = strtok(gunes,":");
            bool sab=0,og=0,ik=0,ak=0,yat=0;
            Serial.printf("%d - %d - %d",saat,atoi(token),(saat>=atoi(token)));
//SABAH
              if(saat>atoi(token)){
               sab = 1;
            }
            else if(saat==atoi(token)){
              token = strtok(NULL, s);
              if(dakika>=atoi(token)) sab = 1;
            }
            else sab = 0;
//OGLE
            token = strtok(oglen,":");  
              if(saat>atoi(token)){
               og = 1;
            }
            else if(saat==atoi(token)){
              token = strtok(NULL, s);
              if(dakika>=atoi(token)) og = 1;
            }
            else og = 0;
//IKINDI
            token = strtok(ikindi,":");  
              if(saat>atoi(token)){
               ik = 1;
            }
            else if(saat==atoi(token)){
              token = strtok(NULL, s);
              if(dakika>=atoi(token)) ik = 1;
            }
            else ik = 0;
//AKSAM
            token = strtok(aksam,":");
              if(saat>atoi(token)){
               ak = 1;
            }
            else if(saat==atoi(token)){
              token = strtok(NULL, s);
              if(dakika>=atoi(token)) ak = 1;
            }
            else ak = 0;
//YATSI
            token = strtok(yatsi,":");  
              if(saat>atoi(token)){
               yat = 1;
            }
            else if(saat==atoi(token)){
              token = strtok(NULL, s);
              if(dakika>=atoi(token)) yat = 1;
            }
            else yat = 0;
          //BURDAN
          if(sab) digitalWrite(SABAH,HIGH);
          else digitalWrite(SABAH,LOW);
          if(og) digitalWrite(OGLE,HIGH);
          else digitalWrite(OGLE,LOW);
          if(ik) digitalWrite(IKINDI,HIGH);
          else digitalWrite(IKINDI,LOW);
          if(ak) digitalWrite(AKSAM,HIGH);
          else digitalWrite(AKSAM,LOW);
          if(yat) digitalWrite(YATSI,HIGH);
          else digitalWrite(YATSI,LOW);
          if(yat == 1){
            token = strtok(yatsi,":");  
              if(saat>atoi(token)){
               //YATSI 22:10'da okunduysa, 23'te kapanır hepsi.
               digitalWrite(SABAH,LOW);
               digitalWrite(OGLE,LOW);
               digitalWrite(IKINDI,LOW);
               digitalWrite(AKSAM,LOW);
               digitalWrite(YATSI,LOW);
            }
            
          }
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  }
  delay(25000);
}

void connectWifi() {
  // Let us connect to WiFi
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println(".......");
  Serial.println("WiFi Connected....IP Address:");
  Serial.println(WiFi.localIP());
 
}
