#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <ESP8266WebServer.h>
//#include <WiFiManager.h>
#include <base64.h>
#include <ArduinoHttpClient.h>
#include <DNSServer.h>


#include "config.hpp"
#include "microHTTP.h"

#define DEBUG
#define CONNECTION_TIMEOUT 10000

#define GPIO0 0 //D3
#define GPIO2 2 //D4

const int ButtonPin = GPIO0;
int buttonState = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, GPIO2, NEO_GRB + NEO_KHZ800);
Ticker checkStatus;
bool queuedCheckToggle = true;
String runningId = "0";
ESP8266WebServer server(80);
bool configured = false;
bool newNetworkSettings = false;
bool newNetworkFound = false;

String newssid;
String newpassword;

Settings conf;
WiFiClientSecure wcs;

bool startNewEntry();
String getCurrentId();
void checkToggl();
void queueCheckToggl();
void stopWork(String);
void configPage();
void startAP();

void setup()
{
    Serial.begin(115200);
    Serial.println("");
    delay(1000);

    pinMode(ButtonPin, INPUT_PULLUP);
    if(digitalRead(ButtonPin) == 0) {
        clearConfig();
        Serial.println("Removed Settings");
    }


    if(readConfig(conf)) {
        // config found, try to connect
        Serial.println("Trying to connect to: " + String(conf.ssid) + " with " + String(conf.password));
        configured = true;
        WiFi.disconnect();
        WiFi.begin(conf.ssid, conf.password);

        unsigned long timeoutBegin = millis();
        bool timeout = false;
        while(WiFi.status() != WL_CONNECTED) {
            if(( millis() - timeoutBegin) > CONNECTION_TIMEOUT) {
                timeout = true;
                Serial.println("Connection failed");
                break;
            }
            delay(100);
        }

        if(timeout) {
            // start AP
            startAP();
            writeConfig(conf);
            configured = true;
        }

        // connected

    } else {
        // no config found, start AP

        // start AP
        startAP();
        writeConfig(conf);
        configured = true;
    }

    strlcpy(conf.password, newpassword.c_str(), sizeof(conf.password));
    strlcpy(conf.ssid, newssid.c_str(), sizeof(conf.ssid));

    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", configPage);
    server.begin();

    strip.begin();
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    checkStatus.attach(30, queueCheckToggl);

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



void handleConfigPage() {
    const String wifiConfigPage = ""
          "<html>"
              "<head>"
                  "<title>Toggl.com Button</title>"
                  "<style>"
                      "body {"
                      "background-color:#fff;font-family:'Arial';display:flex;align-items:center;justify-content:center;height:100vh;color:#222"
                      "}"
                      ".wrapper {"
                      "background-color:#efefef;width:50%;padding:20px;max-width:500px;"
                      "}"
                      ".font-color {color: #f30c16;}"
                      ".bg-color {background-color: #f30c16;}"
                      ".text-center {text-align:center;}"
                      ".flex-column {display:flex;flex-direction:column;}"
                      "h1 {margin-bottom:5px;}"
                      "h3 {margin-bottom:10px}"
                      ".input-container {margin-bottom:20px}"
                      ".input-container label {"
                      "color:#666;margin-bottom:3px;font-size:80%;padding-left:2px;"
                      "}"
                      ".input-container input {"
                      "margin:0;padding:5px 2px;font-size:16px;"
                      "}"
                      "input[type=\"submit\"] {"
                      "border:none;height:50px;color:white;font-size:18px;font-weight:bold;margin-bottom:20px;transition: background-color .3s ease;"
                      "}"
                      "input[type=\"submit\"]:hover {"
                      "cursor: pointer;background-color: #d70a14;"
                      "}"
                  "</style>"
              "</head>"
              "<body>"
                  "<div class=\"wrapper flex-column\">"
                      "<h1 class=\"font-color text-center\">Toggl Button - Network Settings</h1>"
                      "<div>"
                          "<form class=\"pure-form pure-form-stacked flex-column\">"
                              "<div class=\"input-container flex-column\">"
                                  "<label for='ssid'>SSID</label>"
                                  "<input type='text' id='ssid' name='ssid' required />"
                              "</div>"
                              "<div class=\"input-container flex-column\">"
                                  "<label for='password'>Password</label>"
                                  "<input type='text' id='password' name='password' required />"
                              "</div>"
                              "<input class=\"pure-button pure-button-primary bg-color\" type='submit' value='Submit'>"
                          "</form>"
                      "</div>"
                  "</div>"
              "</body>"
          "</html>";

    newssid = server.arg("ssid");
    newpassword = server.arg("password");

    if(newssid != "" && newpassword != "") {
        Serial.println("Trying to connect to: " + String(newssid) + " with " + String(newpassword));
        server.send(200, "text/html", "Trying to connect to new network.");

        newNetworkSettings = true;
    } else {
        server.send(200, "text/html", wifiConfigPage);

        newNetworkSettings = false;
    }

}

void startAP() {
    Serial.println("Start AP");

    IPAddress apIP(192, 168, 1, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));


    DNSServer dnss;
    dnss.start(53, "*", apIP);

    server.onNotFound(handleConfigPage);
    server.begin();

    while(!newNetworkFound) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("TogglButton", "thebutton");

        Serial.print("Soft-AP IP address = ");
        Serial.println(WiFi.softAPIP());

        while (!newNetworkSettings) {
            dnss.processNextRequest();
            server.handleClient();
        }

        newNetworkSettings = false;

        WiFi.mode(WIFI_STA);
        WiFi.begin(newssid.c_str(), newpassword.c_str());

        unsigned long timeoutBegin = millis();
        bool timeout = false;
        while(WiFi.status() != WL_CONNECTED) {
            if(( millis() - timeoutBegin) > CONNECTION_TIMEOUT) {
                timeout = true;
                Serial.println("Timeout!");
                break;
            }

            delay(100);
        }

        if(!timeout) {
            // Connected
            Serial.println("Connected!");
            newNetworkFound = true;
        } else {
            // Not connectable
            Serial.println("Error!");
            WiFi.disconnect();
        }

    }

    server.stop();

    Serial.println("Stop AP");
    WiFi.softAPdisconnect();

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
      if (server.argName(i) == "taskname" && server.arg(i) != "") strlcpy(conf.taskname1, server.arg(i).c_str(), sizeof(conf.taskname1));
      if (server.argName(i) == "wid" && server.arg(i) != "") strlcpy(conf.wid1, server.arg(i).c_str(), sizeof(conf.wid1));
    }

    if(strlen(conf.apikey) > 0 && strlen(conf.taskname1) > 0 && strlen(conf.wid1) > 0) {
        configured = true;
        writeConfig(conf);
        Serial.println("Configuration saved");
        checkToggl();
    }

    String content = String("") +
            "<html>"
              "<head>"
                "<title>Toggl.com Button</title>"
                "<style>"
                  "body {"
                    "background-color:#fff;font-family:'Arial';display:flex;align-items:center;justify-content:center;height:100vh;color:#222"
                  "}"
                  ".wrapper {"
                    "background-color:#efefef;width:50%;padding:20px;max-width:500px;"
                  "}"
                  ".font-color {color: #f30c16;}"
                  ".bg-color {background-color: #f30c16;}"
                  ".text-center {text-align:center;}"
                  ".flex-column {display:flex;flex-direction:column;}"
                  "h1 {margin-bottom:5px;}"
                  "h3 {margin-bottom:10px}"
                  ".input-container {margin-bottom:20px}"
                  ".input-container label {"
                    "color:#666;margin-bottom:3px;font-size:80%;padding-left:2px;"
                  "}"
                  ".input-container input {"
                    "margin:0;padding:5px 2px;font-size:16px;"
                  "}"
                  "input[type=\"submit\"] {"
                    "border:none;height:50px;color:white;font-size:18px;font-weight:bold;margin-bottom:20px;transition: background-color .3s ease;"
                  "}"
                  "input[type=\"submit\"]:hover {"
                    "cursor: pointer;background-color: #d70a14;"
                  "}"
                "</style>"
              "</head>"
              "<body>"
                "<div class=\"wrapper flex-column\">"
                  "<h1 class=\"font-color text-center\">Toggl Button</h1>"
                  "<p class=\"text-center\">Hold button and reconnect power, to change Wifi-Settings.</p>"
                  "<div>"
                      "<h3 class=\"font-color\">Settings</h3>"
                      "<form class=\"pure-form pure-form-stacked flex-column\">"
                        "<div class=\"input-container flex-column\">"
                          "<label for='apikey'>API Key</label>"
                          "<input type='text' id='apikey' name='apikey' value='" +
                          String(conf.apikey) +
                          "' required />"
                        "</div>"
                        "<div class=\"input-container flex-column\">"
                          "<label for='taskname'>Default Task Name</label>"
                          "<input type='text' id='taskname' name='taskname' value='" +
                          String(conf.taskname1) +
                          "' required />"
                        "</div>"
                        "<div class=\"input-container flex-column\">"
                          "<label for='wid'>Default Workspace</label>"
                          "<input type='text' id='wid' name='wid' value='" +
                          String(conf.wid1) +
                          "' required>"
                        "</div>"
                        "<input class=\"pure-button pure-button-primary bg-color\" type='submit' value='Submit'>"
                      "</form>"
                  "</div>"
                "</div>"
              "</body>"
            "</html>";

//    Serial.println(content);

    server.send(200, "text/html", content);


}

bool startNewEntry() {

    String content = "{\"time_entry\":{\"description\":\"";
    content += conf.taskname1;
    content += "\",\"wid\":";
    content += conf.wid1;
    content += ",\"created_with\":\"Arduino\"}}";

    microHTTP mhs = microHTTP(wcs);
    httpResponse resp = mhs.post("www.toggl.com", 443, "/api/v8/time_entries/start", content, "application/json", "96e9a83a29b10059b0604991286dfe7c", "api_token");


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
    httpResponse resp = mh.get("www.toggl.com", 443, "/api/v8/time_entries/current", "96e9a83a29b10059b0604991286dfe7c", "api_token");

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
    httpResponse resp = mh.put("www.toggl.com", 443, "/api/v8/time_entries/" + currentId + "/stop", "", "application/json", "96e9a83a29b10059b0604991286dfe7c", "api_token");

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
