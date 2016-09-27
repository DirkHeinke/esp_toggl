#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include "secret.h"

const int ButtonPin = 5;
int buttonState = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 4, NEO_GRB + NEO_KHZ800);
Ticker checkStatus;
bool queuedCheckToggle = true;

void callIFTTT();
bool startNewEntry();
String getCurrentId();
String togglApiPost(String, String);
String togglApiGet(String);
void checkToggl();
void queueCheckToggl();

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.print("Connecting to: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    pinMode(ButtonPin, INPUT_PULLUP);
    strip.begin();
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    checkStatus.attach(30, queueCheckToggl);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());

}

void loop()
{
    int newButtonState = digitalRead(ButtonPin);
    if(newButtonState != buttonState) {
        buttonState = newButtonState;
        if(newButtonState == 0) {
            Serial.println("Pressed");
            startNewEntry();
        }
    }
    delay(100);

    if(queuedCheckToggle) {
        checkToggl();
        queuedCheckToggle = false;
    }
}

void queueCheckToggl() {
    queuedCheckToggle = true;
}

void checkToggl() {
    Serial.println("Checking Toggl");
    String id = getCurrentId();
    if(id != "0") {
        strip.setPixelColor(0, 255, 0, 0);
        strip.show();
    } else {
        strip.setPixelColor(0, 0, 0, 0);
        strip.show();
    }
}

bool startNewEntry() {

    String content = "{\"time_entry\":{\"description\":\"test entry\",\"wid\":";
    content += workspace;
    content += ",\"created_with\":\"Arduino\"}}\r\n";
    String request = "POST /api/v8/time_entries/start HTTP/1.1\r\n";
    request += "Host: www.toggl.com\r\n";
    request += "Authorization: Basic ";
    request += togglToken;
    request += "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + String(content.length()) + " \r\n\r\n";

    String resp = togglApiPost(request, content);
    int idIndex = resp.indexOf("\"id\"");
    int kommaIndex = resp.indexOf(",", idIndex);
    if(idIndex > 0) {
        String taskId = resp.substring(idIndex + 5, kommaIndex);
        Serial.println("Found taskid:");
        Serial.println(taskId);
    } else {
        Serial.println("No taskid found. Something went wrong!");
    }

}

String getCurrentId() {
    String request = "GET /api/v8/time_entries/current HTTP/1.1\r\n";
    request += "Host: www.toggl.com\r\n";
    request += "Authorization: Basic ";
    request += togglToken;
    request += "\r\n\r\n";

    String resp = togglApiGet(request);
    Serial.println(resp);
    int idIndex = resp.indexOf("\"id\"");
    int kommaIndex = resp.indexOf(",", idIndex);
    if(idIndex > 0) {
        String taskId = resp.substring(idIndex + 5, kommaIndex);
        Serial.println("Found taskid:");
        Serial.println(taskId);
        return taskId;
    } else {
        Serial.println("No taskid found. You are not working");
        return "0";
    }
}

String togglApiGet(String req) {
    return togglApiPost(req, "");
}

String togglApiPost(String req, String content) {
    WiFiClientSecure client;
    if(!client.connect("www.toggl.com", 443)) {
        Serial.println("Connection failed.");
        return "";
    }

    client.print(req);
    client.print(content);
    Serial.println("Requesting....");
    Serial.println(req);
    Serial.println(content);

    int timeout = 0;
    while (client.available() == 0) {
        delay(10);
        if (timeout++ > 200) {
            Serial.println("Timeout! :-(");
            break;
        }; //wait 2s max
    }
    bool isBody = false;
    String body = "";
    while (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        if(line == "" || line == "\r" || line == "\n") {
            isBody = true;
            continue;
        }
        if(isBody) {
            body += line;
        }
    }
    Serial.println("Request finished");
    Serial.print("Body:");
    Serial.print(body);
    Serial.println();
    return body;
}
