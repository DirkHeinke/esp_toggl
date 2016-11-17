#ifndef config_hpp
#define config_hpp

#include <Arduino.h>

struct Settings {
    char apikey[33] = "";
    char taskname[33] = "";
    char wid[33] = "";
};

bool readConfig(Settings &);
void writeConfig(Settings);
void clearConfig();


#endif