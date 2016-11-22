#ifndef config_hpp
#define config_hpp

#include <Arduino.h>

struct Settings {
    char ssid[33] = "";           // max length of 32 chars + \0
    char password[64] = "";       // max length 63 + \0 (WPA2)
    char apikey[33] = "";         // key is 32 char + \0
    char wid1[10] = "";            // current length is 6, so 10 should be enough
    char taskname1[33] = "";       // 32 char might be enough
};

bool readConfig(Settings &);
void writeConfig(Settings);
void clearConfig();


#endif