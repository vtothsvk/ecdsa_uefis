#include <M5StickC.h>

#include "credentials.h"

#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"
#include "mbedtls/pk.h"
#include "mbedtls/ecdsa.h"

int base64_url(uint8_t* out, size_t o_buff_size, size_t* o_len, const uint8_t* in, size_t in_len);
int ecdsa_sign(uint8_t* out, size_t o_buff_size, const uint8_t* digest, size_t dlen, const char* private_key);

void setup(void) {
    Serial.begin(115200);

    Serial.printf("Blink motherfucker!\r\n");

    char buffer[600];
    size_t h64_len;
    size_t index;

    Serial.printf("63+1 encode: %d\r\n", base64_url((uint8_t*)buffer, sizeof(buffer), &h64_len, (uint8_t*)header, strlen(header)));

    buffer[h64_len] = '.';
    size_t c64_len;
    index = h64_len + 1;
    Serial.printf("63+1 encode: %d\r\n", base64_url((uint8_t*)buffer + index, sizeof(buffer) - index, &c64_len, (uint8_t*)claim, strlen(claim)));
    Serial.printf("output: %s\r\n", buffer);    

    index += c64_len;
    

    uint8_t hash[32];
    int ret = mbedtls_sha256_ret((uint8_t*)buffer, index, hash, NULL);

    buffer[index] = '.';
    index++;

    uint8_t signature[64];
    ret = ecdsa_sign(signature, sizeof(signature), hash, 32, private_key);
    Serial.printf("Sign ret: %d\r\n", ret);

    size_t s64_len;
    Serial.printf("63+1 encode: %d\r\n", base64_url((uint8_t*)buffer + index, sizeof(buffer) - index, &s64_len, signature, 64));
    Serial.printf("output: %s\r\n", buffer);
}//setup

void loop(void) {
    
}//loop

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
