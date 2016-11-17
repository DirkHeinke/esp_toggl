#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <base64.h>
#include <ArduinoHttpClient.h>


#include "config.hpp"
#include "microHTTP.h"

#define DEBUG


const int ButtonPin = 5;
int buttonState = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 4, NEO_GRB + NEO_KHZ800);
Ticker checkStatus;
bool queuedCheckToggle = true;
String runningId = "0";
ESP8266WebServer server(80);
bool configured = false;

Settings conf;
WiFiClientSecure wcs;
WiFiClient wc;
HttpClient client = HttpClient(wcs, "toggl.com", 443);

bool startNewEntry();
String getCurrentId();
String togglApiCall(String, String = "");
void checkToggl();
void queueCheckToggl();
void stopWork(String);
void configPage();

void setup()
{
    Serial.begin(115200);
    Serial.println("");
    delay(100);

    WiFiManager wifiManager;
    // if button wifiManager.resetSettings();
    pinMode(ButtonPin, INPUT_PULLUP);
    if(digitalRead(ButtonPin) == 0) {
        wifiManager.resetSettings();
        clearConfig();
        Serial.println("Removed Settings");
    }

    WiFiManagerParameter custom_text("<p>Set all the following parameters or do it later via the integrated webserver</p>");
    wifiManager.addParameter(&custom_text);
    WiFiManagerParameter wmp_apikey("key", "Toggl API Key", "", 32);
    wifiManager.addParameter(&wmp_apikey);
    WiFiManagerParameter wmp_taskname("name", "Default Task Name", "", 32);
    wifiManager.addParameter(&wmp_taskname);
    WiFiManagerParameter wmp_workspace("wid", "Default Workspace ID", "", 32);
    wifiManager.addParameter(&wmp_workspace);

    wifiManager.autoConnect("TogglButton", "tgglbttn");

    server.on("/", configPage);
    server.begin();

    pinMode(ButtonPin, INPUT_PULLUP);
    strip.begin();
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    checkStatus.attach(30, queueCheckToggl);

    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
    if(String(wmp_apikey.getValue()).length() > 0 && String(wmp_taskname.getValue()).length() > 0 && String(wmp_workspace.getValue()).length() > 0) {
        configured = true;
        Serial.println("Configuration saved");

        strlcpy(conf.apikey, wmp_apikey.getValue(), sizeof(conf.apikey));
        strlcpy(conf.taskname, wmp_taskname.getValue(), sizeof(conf.taskname));
        strlcpy(conf.wid, wmp_workspace.getValue(), sizeof(conf.wid));

        writeConfig(conf);
    } else {
        Serial.println("No config found. Trying to load old one");
        bool success = readConfig(conf);
        if(success) configured = true;
    }


}

void loop()
{
    // TODO handle as interrupt
    int newButtonState = digitalRead(ButtonPin);
    if(newButtonState != buttonState) {
        buttonState = newButtonState;
        if(newButtonState == 0) {
            Serial.println("Pressed");
            if(runningId != "0") {
                Serial.println("Stop working");
                stopWork(runningId);
            } else {
                Serial.println("Start working");
                startNewEntry();
            }
        }
    }

    if(queuedCheckToggle) {
        checkToggl();
        queuedCheckToggle = false;
    }

    if(!configured) {
        if((int)(millis() / 1000) % 2 == 0) {
            strip.setPixelColor(0, 255, 255, 0);
            strip.show();
        } else {
            strip.setPixelColor(0, 0, 0, 0);
            strip.show();
        }

    }

    server.handleClient();
}

void queueCheckToggl() {
    queuedCheckToggle = true;
}

void checkToggl() {
    Serial.println("Checking Toggl");
    runningId = getCurrentId();
    if(runningId != "0") {
        strip.setPixelColor(0, 255, 0, 0);
        strip.show();
    } else {
        strip.setPixelColor(0, 0, 0, 0);
        strip.show();
    }
}

void configPage() {

    for (int i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "apikey" && server.arg(i) != "") strlcpy(conf.apikey, server.arg(i).c_str(), sizeof(conf.apikey));
      if (server.argName(i) == "taskname" && server.arg(i) != "") strlcpy(conf.taskname, server.arg(i).c_str(), sizeof(conf.taskname));
      if (server.argName(i) == "wid" && server.arg(i) != "") strlcpy(conf.wid, server.arg(i).c_str(), sizeof(conf.wid));
    }

    if(strlen(conf.apikey) > 0 && strlen(conf.taskname) > 0 && strlen(conf.wid) > 0) {
        configured = true;
        writeConfig(conf);
        Serial.println("Configuration saved");
        checkToggl();
    }

    String content = "<html><head><title>Toggl.com Button</title><head>";
    content += "<body>";
    content += "<p>Hold button and reconnect power, to change WIFI-Settings.</p>";
    content += "<h2>Settings</h2>";
    content += "<form>";
    content += "<p>Toggle API Key: <input type='text' name='apikey' value='";
    content += conf.apikey;
    content += "' required></p>";
    content += "<p>Default Task Name: <input type='text' name='taskname' value='";
    content += conf.taskname;
    content += "' required></p>";
    content += "<p>Default workspace: <input type='text' name='wid' value='";
    content += conf.wid;
    content += "' required></p>";
    content += "<input type='submit' value='Submit'>";
    content += "</form>";
    content += "</body></html>";

    server.send(200, "text/html", content);


}

bool startNewEntry() {

    String content = "{\"time_entry\":{\"description\":\"";
    content += conf.taskname;
    content += "\",\"wid\":";
    content += conf.wid;
    content += ",\"created_with\":\"Arduino\"}}";

    microHTTP mhs = microHTTP(wcs);
//    microHTTP mh = microHTTP(wc);
    httpResponse resp = mhs.post("www.toggl.com", 443, "/api/v8/time_entries/start", content, "application/json", "96e9a83a29b10059b0604991286dfe7c", "api_token");
//    httpResponse respt = mh.post("192.168.178.75", 8080, "/api/v8/time_entries/start", content, "application/json", "96e9a83a29b10059b0604991286dfe7c", "api_token");


    String respBody = resp.body;

    int idIndex = respBody.indexOf("\"id\"");
    int kommaIndex = respBody.indexOf(",", idIndex);
    if(idIndex > 0) {
        String taskId = respBody.substring(idIndex + 5, kommaIndex);
        Serial.println("Found taskid:");
        Serial.println(taskId);
        strip.setPixelColor(0, 255, 0, 0);
        strip.show();
    } else {
        Serial.println("No taskid found. Something went wrong!");
    }

}

String getCurrentId() {
    microHTTP mh = microHTTP(wcs);
//    microHTTP mh = microHTTP(wc);
    httpResponse resp = mh.get("www.toggl.com", 443, "/api/v8/time_entries/current", "96e9a83a29b10059b0604991286dfe7c", "api_token");
//    httpResponse resp = mh.get("192.168.178.75", 8080, "/api/v8/time_entries/current", "96e9a83a29b10059b0604991286dfe7c", "api_token");

    String respBody = resp.body;

    Serial.println(respBody);

    int idIndex = respBody.indexOf("\"id\"");
    int kommaIndex = respBody.indexOf(",", idIndex);

    if(idIndex > 0) {
        String taskId = respBody.substring(idIndex + 5, kommaIndex);
        Serial.println("Found taskid:");
        Serial.println(taskId);
        return taskId;
    } else {
        Serial.println("No taskid found. You are not working");
        return "0";
    }
}

void stopWork(String currentId) {
    microHTTP mh = microHTTP(wcs);
//    microHTTP mh = microHTTP(wc);
    httpResponse resp = mh.put("www.toggl.com", 443, "/api/v8/time_entries/" + currentId + "/stop", "", "application/json", "96e9a83a29b10059b0604991286dfe7c", "api_token");
//    httpResponse resp = mh.put("192.168.178.75", 8080, "/api/v8/time_entries/" + currentId + "/stop", "", "application/json", "96e9a83a29b10059b0604991286dfe7c", "api_token");

    String respBody = resp.body;
    if(resp.status == 200) {
        strip.setPixelColor(0, 0, 0, 0);
        runningId = "0";
        strip.show();
    } else {
        strip.setPixelColor(0, 255, 0, 0);
        strip.show();
    }
}
