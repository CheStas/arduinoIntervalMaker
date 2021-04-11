#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// TODO
// 1. contorl work prosess:
//   - should box open?
//   - should light on?
//   - should press button?
// 2. show lastIntervalTime
// 4. show current process (is at work?)
// 3. show estimated finished time
// 5. connect and test button to make a photo
// 6. light on at, light off at
// 7. button light on/off, show light status
// 8. smooth on/off light
// 9. connect photoresistor (test diff resisot to choose sensetivity), show current ligh status
// 10. adjust light use photoresistor


// ;  to upload data folder:
// ;  pio run -t uploadfs


const long utcOffsetInSeconds = 7200;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
long savedMillisForTime = 0;
unsigned long savedMillisForServo = 0;
long savedEpochTime = 0;
String savedTimeString = "notConnected";
// const int pResistor = D13;
// const int lightSwitcher = D10;


// servo
Servo myservo;
int gatePosition = 0;
int goalGatePosition = 0;
// end servo


// wifi
const char* ssid;
const char* password;
MDNSResponder mdns;
ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
int connectedUsers = 0;
// TODO remove?????
String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)
// end wifi



// Event names
struct EventNames {
  const char connectedUsersEventName[5] = "user";
  const char timeEventName[5] = "time";
  const char gateEventName[5] = "gate";
  const char intervalsEventName[5] = "inte";
  const char workingTimeEventName[5] = "woti";
  const char idleTimeEventName[5] = "idti";
  const char lastIntervalTimeEventName[5] = "lati";
  const char estimatedFinishTimeEventName[5] = "esti";
  const char workStatusEventName[5] = "wost";
  const char isOpenGateAtWorkEventName[5] = "isga";
  const char isLightOnAtWorkEventName[5] = "isli";
  const char isPressButtonAtWorkEventName[5] = "isbu";
  const char goalGatePositionEventName[5] = "gapo";
};
constexpr EventNames eventNames;

// end Event names



// Database
const char nameField[5] = "name";
const char valueField[6] = "value";

String getMessageToSend(const char* fieldName, String value) {
  int size = sizeof(nameField) + sizeof(valueField) + sizeof(fieldName) + sizeof(value);
  DynamicJsonDocument jsonDoc(size*4);
  jsonDoc[nameField] = fieldName;
  jsonDoc[valueField] = value;
  String data;
  serializeJson(jsonDoc, data);
  return data;
}

String getMessageToSend(const char* fieldName, int value) {
  // TODO check !! count size
  int size = sizeof(nameField) + sizeof(valueField) + sizeof(fieldName) + sizeof(value);
  DynamicJsonDocument jsonDoc(size*4);
  jsonDoc[nameField] = fieldName;
  jsonDoc[valueField] = value;
  String data;
  serializeJson(jsonDoc, data);
  return data;
}

// dbdata:
int intervalCounter = 0;
int workingTimeSec = 0;
int idleTimeSec = 0;
int gateStatus = 0;
long lastIntervalTime = 0;
long estimatedFinishTime = 0;
bool isInProgress = false;
bool isOpenGateAtWork = true;
bool isLightOnAtWork = true;
bool isPressButtonAtWork = true;
bool isDataInBuffer = false;


void updateAllDataInDb() {
  File db = SPIFFS.open("/data.json", "w+");
  // if (db.available()) {
     // TODO count doc size!!!
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, db);
  
    jsonDoc[eventNames.gateEventName] = gateStatus;
    jsonDoc[eventNames.intervalsEventName] = intervalCounter;
    jsonDoc[eventNames.workingTimeEventName] = workingTimeSec;
    jsonDoc[eventNames.idleTimeEventName] = idleTimeSec;
    jsonDoc[eventNames.lastIntervalTimeEventName] = lastIntervalTime;
    jsonDoc[eventNames.estimatedFinishTimeEventName] = estimatedFinishTime;
    jsonDoc[eventNames.workStatusEventName] = isInProgress;
    jsonDoc[eventNames.isOpenGateAtWorkEventName] = isOpenGateAtWork;
    jsonDoc[eventNames.isLightOnAtWorkEventName] = isLightOnAtWork;
    jsonDoc[eventNames.isPressButtonAtWorkEventName] = isPressButtonAtWork;
    jsonDoc[eventNames.goalGatePositionEventName] = goalGatePosition;

    serializeJson(jsonDoc, db);

  // }
  db.close();
}

void saveDataInDatabasLoop() {
  if (isDataInBuffer) {
    updateAllDataInDb();
    isDataInBuffer = false;
  }
}

void setDataFromDb() {
  File dataFile = SPIFFS.open("/data.json", "r");
  if (dataFile.available()) {
    DynamicJsonDocument jsonDoc(dataFile.size());
    deserializeJson(jsonDoc, dataFile);

    gateStatus = jsonDoc[eventNames.gateEventName];
    intervalCounter = jsonDoc[eventNames.intervalsEventName];
    workingTimeSec = jsonDoc[eventNames.workingTimeEventName];
    idleTimeSec = jsonDoc[eventNames.idleTimeEventName];
    lastIntervalTime = jsonDoc[eventNames.lastIntervalTimeEventName];
    estimatedFinishTime = jsonDoc[eventNames.estimatedFinishTimeEventName];
    isInProgress = jsonDoc[eventNames.workStatusEventName];
    isOpenGateAtWork = jsonDoc[eventNames.isOpenGateAtWorkEventName];
    isLightOnAtWork = jsonDoc[eventNames.isLightOnAtWorkEventName];
    isPressButtonAtWork = jsonDoc[eventNames.isPressButtonAtWorkEventName];
    goalGatePosition = jsonDoc[eventNames.goalGatePositionEventName];
  }

  dataFile.close();
}

// END Database


// GATE

void closeGate() {
  goalGatePosition = 180;

  isDataInBuffer = true;
}

void openGate() {
  goalGatePosition = 0;

  isDataInBuffer = true;
}

void setGateStatus() {
  gateStatus = myservo.read() == 0 ? 1 : 0;
}

void gateLoop() {
  gatePosition = myservo.read();
  if (gatePosition != goalGatePosition && millis() > savedMillisForServo) {
      if (gatePosition < goalGatePosition) myservo.write(gatePosition+1);
      if (gatePosition > goalGatePosition) myservo.write(gatePosition-1);

      gatePosition = myservo.read();
      if (gatePosition == goalGatePosition) {
        setGateStatus();
        String msg = getMessageToSend(eventNames.gateEventName, gateStatus);
        webSocket.broadcastTXT(msg);
      }
      savedMillisForServo = millis() + 30;
  }
}
// END GATE

// TIME
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

void updateTimeString(String time) {
  savedTimeString = time;
  String msg = getMessageToSend(eventNames.timeEventName, savedTimeString);
  webSocket.broadcastTXT(msg);
}

void updateEpochTime(long time) {
  savedEpochTime = time;
}

void timeLoop() {
  if (millis() - savedMillisForTime > 1000) {
    timeClient.update();
    savedMillisForTime = millis();
    updateTimeString(timeClient.getFormattedTime());
    updateEpochTime(timeClient.getEpochTime());
    Serial.println(savedEpochTime);
  }
}
// END TIME



// INTERVAL COUNTER

// every idleTimeSec startWorking during workingTimeSec, stop after intervalcount time 

void doWork() {
    // working:
  // - open box
  // - turn light on
  // - adjust light
  // - make photo

  openGate();
}

void endWork() {
  // - turn light off
  // - close box
  closeGate();
}

void setInterval(int intervals) {
  intervalCounter = intervals;

  String msg = getMessageToSend(eventNames.intervalsEventName, intervalCounter);
  webSocket.broadcastTXT(msg);

  isDataInBuffer = true;
}

void setWorkingTime(int workTime) {
  workingTimeSec = workTime;

  String msg = getMessageToSend(eventNames.workingTimeEventName, workingTimeSec);
  webSocket.broadcastTXT(msg);

  isDataInBuffer = true;
}

void setIdleTime(int idleTime) {
  idleTimeSec = idleTime;

  String msg = getMessageToSend(eventNames.idleTimeEventName, idleTimeSec);
  webSocket.broadcastTXT(msg);

  isDataInBuffer = true;
}

void setLastIntervalTime(long time) {
  lastIntervalTime = time;

  String msg = getMessageToSend(eventNames.lastIntervalTimeEventName, lastIntervalTime);
  webSocket.broadcastTXT(msg);

  isDataInBuffer = true;
}

void setIsInProgress(bool is) {
  isInProgress = is;

  isDataInBuffer = true;
}

void setIsOpenGateAtWork(bool is) {
  isOpenGateAtWork = is;

  isDataInBuffer = true;
}

void setIsLightOnAtWork(bool is) {
  isLightOnAtWork = is;

  isDataInBuffer = true;
}

void setIsPressButtonAtWork(bool is) {
  isPressButtonAtWork = is;

  isDataInBuffer = true;
}

void intervalCounterLoop() {
  if (intervalCounter && idleTimeSec && workingTimeSec) {
    const int diffFromLastInterval = savedEpochTime - lastIntervalTime;
    if (!isInProgress && diffFromLastInterval > idleTimeSec) {
      // start working
      setIsInProgress(true);
      setLastIntervalTime(savedEpochTime);
      doWork();
    } else if (isInProgress && diffFromLastInterval > workingTimeSec) {
      // stop working
      setIsInProgress(false);
      endWork();
      setLastIntervalTime(savedEpochTime);
      setInterval(--intervalCounter);
    }
  }
}
// END INTERVAL COUNTER

void sendInitialData(uint8_t num) {

  String gateMsg = getMessageToSend(eventNames.gateEventName, gateStatus);
  webSocket.sendTXT(num, gateMsg);

  String intervalMsg = getMessageToSend(eventNames.intervalsEventName, intervalCounter);
  webSocket.sendTXT(num, intervalMsg);

  String workingTimemsg = getMessageToSend(eventNames.workingTimeEventName, workingTimeSec);
  webSocket.sendTXT(num, workingTimemsg);

  String idleTimemsg = getMessageToSend(eventNames.idleTimeEventName, idleTimeSec);
  webSocket.sendTXT(num, idleTimemsg);

  String lastIntervalTimeMsg = getMessageToSend(eventNames.lastIntervalTimeEventName, lastIntervalTime);
  webSocket.sendTXT(num, lastIntervalTimeMsg);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
      Serial.printf("[%u] Disconnected!\r\n", num);
      connectedUsers -= 1;
      String msg = getMessageToSend(eventNames.connectedUsersEventName, connectedUsers);
      webSocket.broadcastTXT(msg);
      break;
    }
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        connectedUsers += 1;
        String msg = getMessageToSend(eventNames.connectedUsersEventName, connectedUsers);
        webSocket.broadcastTXT(msg);

        sendInitialData(num);
        break;
    }
    case WStype_TEXT: {
      char* payloadStrg = (char *)payload;
      Serial.printf("[%u] get Text: %s\r\n", num, payloadStrg);
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payloadStrg);
      const char* name = doc[nameField];
      const int value = doc[valueField];
      if (strcmp(name,eventNames.gateEventName) == 0) {
        gateStatus ? closeGate() : openGate();
      } else
      if (strcmp(name, eventNames.intervalsEventName) == 0) {
        setInterval(value);
      } else
      if (strcmp(name, eventNames.workingTimeEventName) == 0) {
        setWorkingTime(value);
      } else
      if (strcmp(name, eventNames.idleTimeEventName) == 0) {
        setIdleTime(value);
      } else {
        Serial.print("unnkown event name: ");
        Serial.print(name);
        Serial.print(", value: ");
        Serial.println(value);
      }
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


String getContentType(String filename) {
  if(filename.endsWith(".htm")) return "text/html";
  if(filename.endsWith(".html")) return "text/html";
  if(filename.endsWith(".css")) return "text/css";
  if(filename.endsWith(".js")) return "application/javascript";
  if(filename.endsWith(".json")) return "application/json";
  if(filename.endsWith(".svg")) return "image/svg+xml";
  if(filename.endsWith(".png")) return "image/png";
  if(filename.endsWith(".gif")) return "image/gif";
  if(filename.endsWith(".jpg")) return "image/jpeg";
  if(filename.endsWith(".ico")) return "image/x-icon";
  if(filename.endsWith(".xml")) return "text/xml";
  if(filename.endsWith(".pdf")) return "application/x-pdf";
  if(filename.endsWith(".zip")) return "application/x-zip";
  if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {  // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  // If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                         // Use the compressed version
    File file = SPIFFS.open(path, "r");                    // Open the file
    server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;                                          // If the file doesn't exist, return false
}

void wifiSetup() {

  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (mdns.begin("espWebSock", WiFi.localIP())) {
    Serial.println("MDNS responder started");
    mdns.addService("http", "tcp", 80);
    mdns.addService("ws", "tcp", 81);
  } else {
    Serial.println("MDNS.begin failed");
  }
  Serial.print("Connect to http://");
  Serial.println(WiFi.localIP());

}


void setup() {
  Serial.begin(9600);

  wifiSetup();

  SPIFFS.begin();

  server.onNotFound([]() {                              // If the client requests any URI
  if (!handleFileRead(server.uri()))                  // send it if it exists
    server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


  // pinMode(pResistor, OUTPUT);
  // pinMode(lightSwitcher, OUTPUT);

  // MAKE LIGHT NOT BLICK ON CAMERA
  analogWriteFreq(5000);

  // pinMode(pResistor, INPUT);
 
  myservo.attach(D10);

  timeClient.begin();

  // TODO get data from db::
  setDataFromDb();
  Serial.println("setup done");
}

void loop() {

  server.handleClient();
  webSocket.loop();
  timeLoop();
  gateLoop();
  intervalCounterLoop();
  saveDataInDatabasLoop();
}
