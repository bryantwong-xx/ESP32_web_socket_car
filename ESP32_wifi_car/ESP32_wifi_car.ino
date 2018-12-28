#include<WiFi.h>
#include<WebServer.h>
#include<WebSocketsServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include<Servo.h>
//
const char* ssid = "xxxx";
const char* password = "xxxxxxx";

//customize the SSID name and change the password
//const char* ssid     = "ESP32-Robot";
//const char* password = "123456789";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

//pin mode for motor R
int motor1EnablePin = 14;
int motor1pin1 = 27;
int motor1pin2 = 26;

//pin mode for motor L
int motor2EnablePin = 32;
int motor2pin1 = 33;
int motor2pin2 = 25;

//motor PWM channel must be set 2 and 3, rather than 0.
const int freq = 30000;
const int motor1pwmChannel = 2;
const int motor2pwmChannel = 3;
const int resolution = 8;
int dutyCycle = 0; //for speed control

//servo
Servo steeringServo;
int steeringServoPin = 18;

///response from web socket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t*payload, size_t length){
    Serial.printf("webSocketEvent(%d,%d,...)\r\n", num, type);
    switch(type){
      case WStype_DISCONNECTED:
        Serial.printf("[%u]Disconnected!\r\n",num);
        break;
      case WStype_CONNECTED:{
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u]Connected from %d.%d.%d.%d url:%s\r\n",num,ip[0],ip[1],ip[2],ip[3],payload);
        

      }
        break;
      case WStype_TEXT:
        Serial.printf("[%u]get Text: %s\r\n",num,payload);
        //Serial.printf((const char*) payload);
        //strcmp compare 2 strings (need to char*str)
        //if Return value < 0 then it indicates str1 is less than str2.
        //if Return value > 0 then it indicates str2 is less than str1.
        //if Return value = 0 then it indicates str1 is equal to str2.
        if(payload[0] == 'F'){
          //convert to String from payload
            String strPayload = String((const char*) payload);
            //remove 1st char from string
            strPayload.remove(0,1);
          //  Serial.println(strPayload);
          //function to control motors for move forward and backward
          FrontAndBackMoveDirectionControl(strPayload);
           
        }

        if(payload[0] == 'L'){
          //convert to String from payload
            String strPayload = String((const char*) payload);
            //remove 1st char from string
            strPayload.remove(0,1);
            //Serial.println(strPayload);
            //function to control servo motor to rotate left and right 
            steeringDirectionControl(strPayload);
        }

        //send all data to all connected clients/browser
       // webSocket.broadcastTXT(payload, length);
    
        break;
      case WStype_BIN:
        Serial.printf("[%u]get binary length: %u\r\n",num,length);      
        break;
    }   
}

///motor motion
void moveForward(){
  //motor R
  digitalWrite(motor1pin1, LOW); 
  digitalWrite(motor1pin2, HIGH);
  //motor L
  digitalWrite(motor2pin1, LOW); 
  digitalWrite(motor2pin2, HIGH); 

}

void moveBackward(){
  //motor R
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  //motor L
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void moveStop(){
  //motor R
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  //motor L
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);  
}

void FrontAndBackMoveDirectionControl(String directionData){
      //control forward and backward move///
     //convert string to int
    uint8_t Direction = directionData.toInt();
    //map it to be 0-100, and then convert to string
       
    if(Direction > 31){
      moveBackward();
      /// map motor speed from 140 (motor stop) to 255 
      dutyCycle = map(Direction, 31,60,170,255);
      ledcWrite(motor1pwmChannel, dutyCycle);
      ledcWrite(motor2pwmChannel, dutyCycle);
      Serial.print("move backward ->");
      Serial.println(dutyCycle);
    }
    else if(Direction < 29){
      moveForward();
      /// map motor speed from 140 (motor stop) to 255 
      dutyCycle = map(Direction, 29,0,170,255);
      ledcWrite(motor1pwmChannel, dutyCycle);
      ledcWrite(motor2pwmChannel, dutyCycle);
       Serial.print("move forward ->");
       Serial.println(dutyCycle);

    }

    else{
      Serial.println("move stop");
        moveStop();
    }
       

    
}
int steeringAngle = 0;
void steeringDirectionControl(String SteeringData){
  //control servo motor to steering//
  Serial.print("servo direction");
  Serial.println(SteeringData);
  int Steering = SteeringData.toInt();
 
     if(Steering > 30){
      Serial.println("move right");
      Serial.println(90+Steering-15);//my servo motor place offset...I made the steering angle trickly..steering angle is 55 to 135
      steeringAngle = map(Steering, 30,60,90,135);
      steeringServo.write(steeringAngle);

    }
    else if(Steering < 30){
      Serial.println("move left");

      Serial.println(90-(35-Steering));//my servo motor place offset...I made the steering angle trickly..steering angle is 55 to 135
      steeringAngle = map(Steering, 30,0,90,55);
      steeringServo.write(steeringAngle);

    } 
}

void setup() {
  Serial.begin(115200);
   //set up servo
   steeringServo.attach(steeringServoPin);
   //directionServo.write(90);

  //set up motors
  pinMode(motor1EnablePin, OUTPUT);
  pinMode(motor1pin1, OUTPUT); 
  pinMode(motor1pin2, OUTPUT);  

  pinMode(motor2EnablePin, OUTPUT);
  pinMode(motor2pin1, OUTPUT); 
  pinMode(motor2pin2, OUTPUT); 
  
  
  //configure PWM channel 
  ledcSetup(motor1pwmChannel,freq, resolution);
  ledcSetup(motor2pwmChannel,freq, resolution);
  
  // Attach the PWM channel to the enable pins which are the GPIOs to be controlled
  ledcAttachPin(motor1EnablePin, motor1pwmChannel);
  ledcAttachPin(motor2EnablePin, motor2pwmChannel);
  
  // Produce a PWM signal to both enable pins with a duty cycle 0
  ledcWrite(motor1pwmChannel, dutyCycle);
  ledcWrite(motor2pwmChannel, dutyCycle);  
  /*
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)...");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  */
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  IPAddress myIP = WiFi.localIP();


  //initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("Error while mounting SPIFFS");
    return;
  }
  //mount SPIFFS
  SPIFFS.begin();

  //server.on("/",serveIndexFile);
  //start web socket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
 // Configure web server to host HTML files
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {
  webSocket.loop();
  server.handleClient();
}


// A function we use to get the content type for our HTTP responses
String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

// Takes a URL (for example /index.html) and looks up the file in our file system,
// Then sends it off via the HTTP server!
bool handleFileRead(String path){
  Serial.println(path);
  #ifdef DEBUG
    Serial.println("handleFileRead: " + path);
  #endif
  if(path.endsWith("/")) path += "car.html";
  if(SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false;
}
