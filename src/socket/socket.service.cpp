#include <Arduino.h>
#include <WebSocketsServer.h>
#include <socket/socket.service.h>
#include <common/event_names.h>

void SocketService::begin() {
    webSocket.begin();
    webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
       webSocketEvent(num, type, payload, length);
  });
}

void SocketService::loop() {
    webSocket.loop();
}

void SocketService::broadcastTXT(const char* fieldName, int value) {
    String msg = helper.getMessageToSend(fieldName, value);
    webSocket.broadcastTXT(msg);
}

void SocketService::broadcastTXT(const char* fieldName, String value) {
    String msg = helper.getMessageToSend(fieldName, value);
    webSocket.broadcastTXT(msg);
}

void SocketService::sendTo(uint8_t num, const char* fieldName, int value) {
    String msg = helper.getMessageToSend(fieldName, value);
    webSocket.sendTXT(num, msg);
}

void SocketService::sendTo(uint8_t num, const char* fieldName, String value) {
    String msg = helper.getMessageToSend(fieldName, value);
    webSocket.sendTXT(num, msg);
}

void SocketService::webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED: {
            Serial.printf("[%u] Disconnected!\r\n", num);
            connectedUsers -= 1;
            broadcastTXT(eventNames.connectedUsersEventName, connectedUsers);
            break;
        }
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
            connectedUsers += 1;
            this->mediator_->Notify(this, eventNames.newUsersEventName, num);
            broadcastTXT(eventNames.connectedUsersEventName, connectedUsers);
            break;
        }
        case WStype_TEXT: {
            char *payloadStrg = (char *) payload;
            Serial.printf("[%u] get Text: %s\r\n", num, payloadStrg);
            struct parsedData upSocketData = helper.parseAndGetNameValue(payloadStrg);
            this->mediator_->Notify(this, eventNames.clientNewEventName, upSocketData.name, upSocketData.value);
            break;
        }
        case WStype_BIN: {
            Serial.printf("[%u] get binary length: %u\r\n", num, length);
            hexdump(payload, length);
            // echo data back to browser
            webSocket.sendBIN(num, payload, length);
            break;
        }
        default: {
            Serial.printf("Invalid WStype [%d]\r\n", type);
            break;
        }
    }
}
