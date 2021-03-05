#ifndef CREDENTIALS_H
#define CREDENTIALS_H

const char private_key[] = 
"-----BEGIN EC PRIVATE KEY-----\n" 
"MHcCAQEEIAMLyN6ZrlN6t1M/zExoBDa45IHLcq1wf1iEvJXJs4RFoAoGCCqGSM49\n" 
"AwEHoUQDQgAEvRbRZdlYoNjxfNBlmz2pvKLNBx338acIc8HVVel/+tgajPoiIKdE\n" 
"xLkEGXaN+kigf6EKgm1C/qFx6GmtDucBXg==\n" 
"-----END EC PRIVATE KEY-----\n";
/*
char header[200] = "{\
  \"alg\": \"ES256\",\
  \"typ\": \"JWT\",\
  \"kid\": \"73c326f9669e369662fe3ce1fabf9df32cc87eca0780795c4773fedd4f57f9b5\"\
}";
*/
char claim[200] = "{\
  \"iat\": 1614259801,\
  \"exp\": 1614269801,\
  \"sub\": \"a933af5dd3b16b22\"\
}";



const char* serial_num = "a933af5dd3b16b22";
char* server_name = "https://fei.edu.r-das.sk:51415/api/v1/Auth";
const char* ssid       = "ADB-CFF9A1";
const char* password   = "rce6bn743cjr";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
const int   sec_to_expire = 6000;


#endif
