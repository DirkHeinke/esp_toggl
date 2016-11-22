#include <Arduino.h>
#include <EEPROM.h>
#include "config.hpp"

/**
 * EEPROM Layout
 *  0-2: CFG or XXX
 *  3-35: ssid
 *  36-99: password
 *  100-132: apikey
 *  133-142: wid1
 *  143-175 taskname1
 */

#define EEPROM_SIZE 176

bool readConfig(Settings &conf) {
    EEPROM.begin(EEPROM_SIZE);
    if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' ) {

#ifdef DEBUG
        Serial.println("Found config");
#endif

        char ssid[33] = "";
        char password[64] = "";
        char apikey[33] = "";
        char wid1[10] = "";
        char taskname1[33] = "";

        for (int i = 3; i < EEPROM_SIZE; ++i) {
            if(i < 36) {
                ssid[i-3] = EEPROM.read(i);
            } else if(i < 100) {
                password[i-36] = EEPROM.read(i);
            } else if(i < 133) {
                apikey[i-100] = EEPROM.read(i);
            } else if(i < 143) {
                wid1[i-133] = EEPROM.read(i);
            } else if(i < 176) {
                taskname1[i-143] = EEPROM.read(i);
            }
        }

//        Serial.println(ssid);
//        Serial.println(password);
//        Serial.println(apikey);
//        Serial.println(wid1);
//        Serial.println(taskname1);


        strlcpy(conf.ssid, ssid, sizeof(conf.ssid));
        strlcpy(conf.password, password, sizeof(conf.password));
        strlcpy(conf.apikey, apikey, sizeof(conf.apikey));
        strlcpy(conf.wid1, wid1, sizeof(conf.wid1));
        strlcpy(conf.taskname1, taskname1, sizeof(conf.taskname1));


        EEPROM.end();

        return true;
    } else {
        Serial.println("No config found");
        EEPROM.end();

        return false;
    }
}

void writeConfig(Settings conf) {
    EEPROM.begin(EEPROM_SIZE);

    EEPROM.write(0, 'C');
    EEPROM.write(1, 'F');
    EEPROM.write(2, 'G');

    for (int i = 3; i < EEPROM_SIZE; ++i) {
        if(i < 36) {
            EEPROM.write(i, conf.ssid[i-3]);
        } else if(i < 100) {
            EEPROM.write(i, conf.password[i-36]);
        } else if(i < 133) {
            EEPROM.write(i, conf.apikey[i-100]);
        } else if(i < 143) {
            EEPROM.write(i, conf.wid1[i-133]);
        } else if(i < 176) {
            EEPROM.write(i, conf.taskname1[i-143]);
        }
    }

    EEPROM.end();
}

void clearConfig() {
    EEPROM.begin(EEPROM_SIZE);

    EEPROM.write(0, 'X');
    EEPROM.write(1, 'X');
    EEPROM.write(2, 'X');

    EEPROM.end();
}
