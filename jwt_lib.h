#ifndef JWT_H
#define JWT_H

int base64_url(uint8_t* out, size_t o_buff_size, size_t* o_len, const uint8_t* in, size_t in_len);
int ecdsa_sign(uint8_t* out, size_t o_buff_size, const uint8_t* digest, size_t dlen, const char* private_key);
int Bearer_JWT_Token(const char* private_key, char* kid, char* serial_num, int time_iat, int time_exp,char* bearer_token);

#endif
