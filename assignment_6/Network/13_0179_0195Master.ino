#include <Wire.h>
//Master
char token[10] = "Token#0#";
int target = '3';
bool trigger = false;
bool send = false;
bool fsend = false;
int slave_1 = 1;
int slave_2 = 2;
String freceived = "";

void setup() {
  Wire.begin(3);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600); 
}


void loop() {
  Serial.println(token);
  for(int i=1;i<3;i++){
    String Username;
    Username = Serial.readString();
    
    
    if(Username[8] == '2'){
     Wire.beginTransmission(slave_2); 
    Serial.print("Master received: ");
    for(int i=0;i < Username.length();i++){
      Wire.write(Username[i]);
        Serial.print(Username[i]);
        trigger = true;
    }
    if(trigger){
      Serial.println();
    }
    Wire.endTransmission();    
    delay(500);
    trigger = true;
    }
    
    if(Username[8] == '1'){
    Wire.beginTransmission(slave_1);
    Serial.print("Master received: ");
    for(int i=0;i < Username.length();i++){
      Wire.write(Username[i]);
        Serial.print(Username[i]);
        trigger = true;
    }
    if(trigger){
      Serial.println(); 
    }
    Wire.endTransmission();    
    delay(500);
    trigger = true;
    }
    /////////
    if(trigger && Username[8] == '2'){
    Wire.requestFrom(slave_2, 1);  
    while (Wire.available()) {
      char c = Wire.read();
        Serial.println(token);
      //Serial.println(c);        
    }
      trigger = false;
    }
    
    if(trigger && Username[8] == '1'){
    Wire.requestFrom(slave_1, 1);   
    while (Wire.available()) {
      char c = Wire.read();
        Serial.println(token);      
    }
      trigger = false;
    }
  }
}


void receiveEvent(int howMany) { 
  String received = "";
  while (Wire.available()>0) { 
    char ch = Wire.read();
    received += ch;  
  }
  if(received[6] == '1' && received[8] == '3'){
    Serial.print("Master received from: ");
    Serial.println(received);
    Serial.println(token);
    send = true;
  }
  if(received[8] == '2' ||received[8] == '1' ){
    
    Serial.print("received from slave: ");
    Serial.println(received);
    Serial.println("Token#1");
    send = true;
    fsend = true;
    freceived = received;
  }
}


void requestEvent() {
  if(send){
    Wire.write(token);
    send = true;
  } 
  
  if(fsend){
    Wire.beginTransmission(slave_2);
    for(int n=0;n<freceived.length();n++){
      Wire.write(freceived[n]);
    }
    Wire.endTransmission();    
    freceived = "";
    fsend = false;
  }
}
