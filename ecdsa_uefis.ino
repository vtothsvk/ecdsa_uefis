#include "jwt_lib.h"
#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "credentials.h"



void LocalTime();
 
char* payload = "[{\"LoggerName\": \"Pot\",\"Timestamp\": 1614265180,\"MeasuredData\": [{\"Name\": \"napatie\",\"Value\": 50}],\"ServiceData\": [],\"DebugData\": [],\"DeviceId\": \"08d8d99d-d947-4aaa-88bf-741908951af7\"}]"; 
//test payload

void setup(void) {
    
    time_t time_iat;
    time_t time_exp;
    struct tm timeinfo;                                         //Time variables 


    Serial.begin(115200);             
    Serial.printf("Mehehe!\r\n");                               //Serial init
    
                            
    WiFi.mode(WIFI_STA);    //Wif|I connect
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
        delay(100);
        Serial.print(".");
    }
    Serial.println("Wifi OK!");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);    //Time test
    LocalTime();  
  
    while(1)
    {
      time(&time_iat);                                            //get fresh time for timestamp
      time_exp = time_iat + sec_to_expire;

      HTTPClient http;                                                                                      //Initialize HTTP Client
      http.begin(server_name);

      http.addHeader("Content-Type","application/json");
      http.addHeader("Authorization",(char*)Bearer_JWT_Token(private_key, kid, serial_num, time_iat, time_exp));
     // char* token = Bearer_JWT_Token(private_key, kid, serial_num, time_iat, time_exp);
      //Serial.printf((char*)token);                                                                             
      int http_response = http.POST((char*)payload);
      Serial.print("\n HTTP Code ");                                          
      Serial.print(http_response);
      String server_response = http.getString();
      Serial.print("\nServer response ");
      Serial.print(server_response);
      http.end();
      delay(1000);
    }//while
}//setup


void loop(void) {
    
}//loop

void LocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%H:%M:%S");
    Serial.println("Timestamp: ");
    time_t timestamp;
    time(&timestamp);
}//LocalTime
