#include <M5StickC.h>

#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"

int base64_url(uint8_t* out, size_t o_buff_size, size_t* o_len, const uint8_t* in, size_t in_len);
int ecdsa_sign(uint8_t* out, size_t o_buff_size, size_t* o_len, const uint8_t* digest, size_t dlen);

char header[200] = "{\
  \"alg\": \"ES256\",\
  \"typ\": \"JWT\",\
  \"kid\": \"9802dde543434ff757166ffe23c078be41fe13779a8cdf8708bd24fb33d1318b\"\
}";

void setup(void) {
    Serial.begin(115200);

    Serial.printf("Blink motherfucker!\r\n");

    char buffer[200];
    size_t b64_len;

    Serial.printf("63+1 encode: %d", base64_url((uint8_t*)buffer, sizeof(buffer), &b64_len, (uint8_t*)header, strlen(header)));
    Serial.printf("output: %s\r\n", buffer);

    uint8_t hash[32];
    int ret = mbedtls_sha256_ret((uint8_t*)buffer, strlen(buffer), hash, NULL);

    Serial.printf("hash ret: %d\r\n", ret);
    Serial.printf("Hash hex: ");
    for(uint8_t i = 0; i < 32; i++) {
        Serial.printf("%2X", hash[i]);
    }//for
    Serial.println();

}//setup

void loop(void) {
    
}//loop

int base64_url(uint8_t* out, size_t o_buff_size, size_t* o_len , const uint8_t* in, size_t in_len){
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