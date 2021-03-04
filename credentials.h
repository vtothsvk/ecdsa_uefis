#ifndef CREDENTIALS_H
#define CREDENTIALS_H

const char private_key[] = 
"PASTE PRIVATE KEY HERE";

char header[200] = "{\
  \"alg\": \"ES256\",\
  \"typ\": \"JWT\",\
  \"kid\": \"9802dde543434ff757166ffe23c078be41fe13779a8cdf8708bd24fb33d1318b\"\
}";

char claim[200] = "{\
  \"iat\": 1614259801,\
  \"exp\": 1614269801,\
  \"sub\": \"b952d57d787df2d7\"\
}";

#endif