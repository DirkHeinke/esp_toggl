//
// Created by dirk on 11/10/16.
//

#ifndef ESP_TOGGL_CLION_MICROHTTP_H
#define ESP_TOGGL_CLION_MICROHTTP_H

#include <Arduino.h>
#include <Client.h>

#define HTTP_TIMEOUT_CODE -1
#define HTTP_INVALID_CODE -2

struct httpResponse {
    String header;
    String body;
    int status;
};

class microHTTP {

public:
    microHTTP(Client& client);

    void setTimeout(unsigned short timeInMs);
    unsigned short getTimeout();

    httpResponse get(String host, int port, String path, String user = "", String password = "");
    httpResponse post(String host, int port, String path, String data, String dataType, String user = "", String password = "");
    httpResponse put(String host, int port, String path, String data, String dataType, String user = "", String password = "");

private:
    Client* c;
    unsigned short response_timeout = 10000;

    void sendHeader(String httpMethod, String path, String host);
    void sendAuthHeader(String user, String password);
    void sendCustomHeader(String);
    void SendBody(String data);
    httpResponse readFullResponse();
    String encodeBase64(String data);
};

#endif //ESP_TOGGL_CLION_MICROHTTP_H
