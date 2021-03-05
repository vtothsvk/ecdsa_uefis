#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "credentials.h"

#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/ecdsa.h"

int base64_url(uint8_t* out, size_t o_buff_size, size_t* o_len, const uint8_t* in, size_t in_len);
int ecdsa_sign(uint8_t* out, size_t o_buff_size, const uint8_t* digest, size_t dlen, const char* private_key);
int send_to_server(const char *server_name, char* kid, char* serial_num, int time_iat, int time_exp, char* payload); 
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
      
      int send_ret = send_to_server(server_name, kid, serial_num, time_iat, time_exp, payload);
      Serial.print("\nSend RET ->");
      Serial.print(send_ret);
      delay(1000);
    }//while
}//setup


void loop(void) {
    
}//loop


int send_to_server(const char *server_name, char* kid, char* serial_num, int time_iat, int time_exp, char* payload)    
    {

    char buffer[600];
    size_t h64_len;
    size_t c64_len;
    size_t s64_len;
    size_t index;                                                                                         //JWT Token variables

    HTTPClient http;                                                                                      //Initialize HTTP Client
    http.begin(server_name);

    char header[200];
    sprintf(header,"{\"alg\":\"ES256\",\"typ\":\"JWT\",\"kid\":\"%s\"}",kid);                             //Generate header
    base64_url((uint8_t*)buffer, sizeof(buffer), &h64_len, (uint8_t*)header, strlen(header));             //OK
    
    Serial.printf("63+1 encode: %s\r\n",buffer);
    buffer[h64_len] = '.';
    index = h64_len + 1;
    
    char claim[200];   
    sprintf(claim,"{\"sub\":\"%s\",\"iat\":%ld,\"exp\":%ld}",(char*)serial_num,(char*)time_iat,(char*)time_exp);   //Generate claim
    base64_url((uint8_t*)buffer + index, sizeof(buffer) - index, &c64_len, (uint8_t*)claim, strlen(claim)); //OK
    Serial.printf("output2: %s\r\n", buffer);
    
    
    index += c64_len;
    uint8_t hash[32];
    int ret = mbedtls_sha256_ret((uint8_t*)buffer, index, hash, NULL);                                      //Generate HASH SHA256

    buffer[index] = '.';
    index++;

    uint8_t signature[64];
    ret = ecdsa_sign(signature, sizeof(signature), hash, 32, private_key);
    Serial.printf("Sign ret: %d\r\n", ret);

    base64_url((uint8_t*)buffer + index, sizeof(buffer) - index, &s64_len, signature, 64);                   //Completize JWT Token
    Serial.printf("output: %s\r\n", buffer);

    char bearer_token[250];                                                                                 //Make Bearer Token
    sprintf(bearer_token,"%s%s","Bearer ", (char*)buffer);
  //Serial.print("\nBearer token -> ");
  //Serial.print("\n");
    Serial.println((char*)bearer_token);
   
    http.addHeader("Content-Type","application/json");
    http.addHeader("Authorization",(char*)bearer_token);                                                                             
    int http_response = http.POST((char*)payload);
    Serial.print("\n HTTP Code ");                                          
    Serial.print(http_response);
    String server_response = http.getString();
    Serial.print("\nServer response ");
    Serial.print(server_response);
    http.end();
    return http_response;
  }// send_to_server
  

int base64_url(uint8_t* out, size_t o_buff_size, size_t* o_len, const uint8_t* in, size_t in_len){
    //size_t index;
    size_t e_len;

    int ret = mbedtls_base64_encode(out, o_buff_size, &e_len, in, in_len);
    if (ret) return ret;
    
    for (uint8_t index = 0; index < e_len; index++) {
        if (out[index] == '+') {
            out[index] = '-';
        } else if (out[index] == '/') {
            out[index] = '_';
        } else if (out[index] == '=') {
            out[index] = 0;
            e_len = index;
        }//if
    }//for (uint8_t index = 0; index < elen; index++)

    *o_len = e_len;

    return ret;
}//base64_url


int ecdsa_sign(uint8_t* out, size_t o_buff_size, const uint8_t* digest, size_t dlen, const char* private_key) {

    mbedtls_pk_context pk_ctx;

    mbedtls_pk_init(&pk_ctx);
    int ret = mbedtls_pk_parse_key(&pk_ctx, (unsigned char*)private_key, strlen(private_key) + 1, NULL, 0);
    if (ret) return ret;

    mbedtls_ecdsa_context ecdsa;

    mbedtls_ecdsa_init(&ecdsa);
    ret = mbedtls_ecdsa_from_keypair(&ecdsa, mbedtls_pk_ec(pk_ctx));
    if (ret) return ret;

    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    ret = mbedtls_ecdsa_sign_det(&ecdsa.grp, &r, &s, &ecdsa.d, digest, dlen, MBEDTLS_MD_SHA256);
    if (ret) return ret;

    ret = mbedtls_mpi_write_binary(&r, out, mbedtls_mpi_size(&r));
    if (ret) return ret;

    ret = mbedtls_mpi_write_binary(&s, out + mbedtls_mpi_size(&r), mbedtls_mpi_size(&s));
    if(ret) return ret;

    size_t siglen = mbedtls_mpi_size(&r) + mbedtls_mpi_size(&s);

    if(siglen > o_buff_size) {
        return -1;
    }

    mbedtls_pk_free(&pk_ctx);
    mbedtls_ecdsa_free(&ecdsa);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    return ret;
}//ecdsa_sign




void LocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
    {   
      Serial.println("Failed to obtain time");
      return;
    }
  Serial.println(&timeinfo, "%H:%M:%S");
  Serial.println("Timestamp: ");
  time_t timestamp;
  time(&timestamp);
}//LocalTime
