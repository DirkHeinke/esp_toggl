//
// Created by dirk on 11/10/16.
//

#include "microHTTP.h"

#define MICROHTTP_DEBUG

microHTTP::microHTTP(Client& client) {
    c = &client;
}

void microHTTP::setTimeout(unsigned short timeInMs) {
    response_timeout = timeInMs;
}

unsigned short microHTTP::getTimeout() {
    return response_timeout;
}

httpResponse microHTTP::get(String host, int port, String path, String user, String password) {
#ifdef MICROHTTP_DEBUG
    Serial.println("Call of get");
#endif
    c->connect(host.c_str(), port);
    sendHeader("GET", path, host);
    if(user && password) {
        sendAuthHeader(user, password);
    }
    c->print("\r\n\r\n");
    c->flush();

    httpResponse response = readFullResponse();

#ifdef MICROHTTP_DEBUG
    Serial.println("Header");
    Serial.println(response.header);
    Serial.println("Body");
    Serial.println(response.body);
#endif

    return response;
}

httpResponse microHTTP::post(String host, int port, String path, String data, String dataType, String user, String password) {
#ifdef MICROHTTP_DEBUG
    Serial.println("Call of post");
#endif
    c->connect(host.c_str(), port);
    sendHeader("POST", path, host);
    if(user && password) {
        sendAuthHeader(user, password);
    }

    c->println("Content-Type: " + dataType);
    if(data.length() > 0){
        c->println("Content-Length: " + String(data.length()));
        c->println("");
        c->print(data);
    }

    c->flush();

    httpResponse response = readFullResponse();

#ifdef MICROHTTP_DEBUG
    Serial.println("Header");
    Serial.println(response.header);
    Serial.println("Body");
    Serial.println(response.body);
#endif

    return response;
}

httpResponse microHTTP::put(String host, int port, String path, String data, String dataType, String user,
                             String password) {
#ifdef MICROHTTP_DEBUG
    Serial.println("Call of put");
#endif
    c->connect(host.c_str(), port);
    sendHeader("PUT", path, host);
    if(user && password) {
        sendAuthHeader(user, password);
    }

    c->println("Content-Type: " + dataType);
    if(data.length() > 0){
        c->println("Content-Length: " + String(data.length()));
        c->println("");
        c->print(data);
    }

    c->print("\r\n\r\n");
    c->flush();

    httpResponse response = readFullResponse();

#ifdef MICROHTTP_DEBUG
    Serial.println("Header");
    Serial.println(response.header);
    Serial.println("Body");
    Serial.println(response.body);
#endif

    return response;
}

void microHTTP::sendHeader(String httpMethod, String path, String host) {
    c->print(httpMethod);
    c->print(" ");
    c->print(path);
    c->println(" HTTP/1.1");
    c->print("Host: ");
    c->println(host);
    c->println("User-Agent: microHTTP/0.1");
}

void microHTTP::sendAuthHeader(String user, String password) {
#ifdef MICROHTTP_DEBUG
    Serial.println("Call of AuthHeader");
#endif
    c->print("Authorization: Basic ");
    c->println(encodeBase64(user + ":" + password));
}

httpResponse microHTTP::readFullResponse() {
#ifdef MICROHTTP_DEBUG
    Serial.println("Call of Full Response");
#endif
    String data;
    unsigned long timeoutBegin = millis();
    httpResponse resp;
    bool body = false;

    while(!c->available() && ( millis() - timeoutBegin) < response_timeout ) {
        delay(100);
    }

    // Timeout or Data Available

    if(c->available()) {
        while(c->available()) {
            char character = (char)c->read();
            data += character;
            int dataLength = data.length();

            if(dataLength == 12 && !body) {
                if(data.startsWith("HTTP/1.1")) {
                    resp.status = data.substring(9, 12).toInt();
                } else {
                    // No valid HTTP Response
                    resp.status = HTTP_INVALID_CODE;
#ifdef MICROHTTP_DEBUG
                    Serial.println("Invalid HTTP Response");
                    Serial.println(data);
#endif
                }
            }

            // detect double line break between header and body
            if(dataLength > 4 &&
                    !body &&
                    data.charAt(dataLength - 4) == '\r' && data.charAt(dataLength - 3) == '\n' &&
                    data.charAt(dataLength - 2) == '\r' && data.charAt(dataLength - 1) == '\n') {
                body = true;
                resp.header = data.substring(0,dataLength-4);
                data = "";
            }
        }

        resp.body = data;
    } else {
        // TIMEOUT
        resp.status = HTTP_TIMEOUT_CODE;
#ifdef MICROHTTP_DEBUG
        Serial.println("Timeout of Response");
#endif
    }

    return resp;
}

String microHTTP::encodeBase64(String data) {
    const char* dictionary = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    dictionary += 0x00;
    dictionary += 0x00;
    String encoded = "";

    for (int i = 0; i < data.length(); i+=3) {

        byte s1 = (data[i] & 0b11111100) >> 2;
        byte s2 = (data[i] & 0b00000011) << 4 | (data[i+1] & 0b11110000) >> 4;
        byte s3 = (data[i+1] & 0b00001111) << 2 |(data[i+2] & 0b11000000) >> 6;
        byte s4 = (data[i+2] & 0b00111111);

        encoded += dictionary[s1];
        encoded += dictionary[s2];
        encoded += dictionary[s3];
        encoded += dictionary[s4];

        if(data.length() - 1 == i) {
            encoded.setCharAt(encoded.length() - 1, '=');
            encoded.setCharAt(encoded.length() - 2, '=');
        } else if(data.length() - 2 == i) {
            encoded.setCharAt(encoded.length() - 1, '=');
        }

    }

    return encoded;
}