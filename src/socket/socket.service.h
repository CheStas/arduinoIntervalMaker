#ifndef SOCKET_H
#define SOCKET_H

#include <Arduino.h>
#include <WebSocketsServer.h>
#include <socket/messages.helper.h>
#include <common/mediator.cpp>

class SocketService : public BaseComponent {
public:
    void begin();
    void loop();
    void broadcastTXT(const char* fieldName, int value);
    void broadcastTXT(const char* fieldName, String value);
    void sendTo(uint8_t num, const char* fieldName, int value);
    void sendTo(uint8_t num, const char* fieldName, String value);

private:
    WebSocketsServer webSocket = WebSocketsServer(81);
    int connectedUsers = 0;
    MessageHelper helper;

    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
};

#endif
