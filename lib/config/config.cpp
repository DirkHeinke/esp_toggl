#include <Arduino.h>
#include <EEPROM.h>
#include "config.hpp"

/**
 * EEPROM Layout
 *  0-2: CFG or XXX
 *  3-35: apikey
 *  36-68: taskname
 *  69-102: wid
 */

bool readConfig(Settings &conf) {
    EEPROM.begin(103);
    if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' ) {

#ifdef DEBUG
        Serial.println("Found config");
#endif

        char apikey[33];
        char taskname[33];
        char wid[33];
        for (int i = 0; i < 33; ++i) {
            apikey[i] = EEPROM.read(i+3);
            taskname[i] = EEPROM.read(i+36);
            wid[i] = EEPROM.read(i+69);
        }


        strlcpy(conf.apikey, apikey, sizeof(conf.apikey));
        strlcpy(conf.taskname, taskname, sizeof(conf.taskname));
        strlcpy(conf.wid, wid, sizeof(conf.wid));

#ifdef DEBUG
        Serial.println(conf.apikey);
        Serial.println(conf.taskname);
        Serial.println(conf.wid);
#endif

        EEPROM.end();

        return true;
    } else {
        Serial.println("No config found");
        EEPROM.end();

        return false;
    }
}

void writeConfig(Settings conf) {
    EEPROM.begin(103);

    EEPROM.write(0, 'C');
    EEPROM.write(1, 'F');
    EEPROM.write(2, 'G');

    for (int i = 0; i < 33; ++i) {
        EEPROM.write(i+3, conf.apikey[i]);
        EEPROM.write(i+36, conf.taskname[i]);
        EEPROM.write(i+69, conf.wid[i]);
    }

    EEPROM.end();
}

void clearConfig() {
    EEPROM.begin(103);

    EEPROM.write(0, 'X');
    EEPROM.write(1, 'X');
    EEPROM.write(2, 'X');

    EEPROM.end();
}
