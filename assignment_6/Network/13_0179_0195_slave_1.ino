#include <Wire.h>
//slave_1
int target= '1';
bool send = false;
char token[10] = "Token#0#"; 
bool trigger = false;
int slave_2 = 2;
int master = 3;



void setup() {
  Wire.begin(1);                
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  
  for(int i=1;i<3;i++){
    String Username;
    Username = Serial.readString();
    
    if(Username[8] == '3'){
     Wire.beginTransmission(master); 
    Serial.print("slave received: ");
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
    ///,.,.
    if(Username[8] == '2'){
    Wire.beginTransmission(slave_2);
    Serial.print("slave received: ");
    for(int i=0;i < Username.length();i++){
      Wire.write(Username[i]);
        Serial.print(Username[i]);
        trigger = true;
    }
    if(trigger){
      Serial.println(); 
    }
    Wire.endTransmission();
    
    
    Wire.beginTransmission(master);
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
    if(trigger && Username[8] == '3'){
    Wire.requestFrom(master, 1);  
    while (Wire.available()) {
      char c = Wire.read();
        Serial.println(token);
      //Serial.println(c);        
    }
      trigger = false;
    }
    
    else if(trigger && Username[8] == '2'){
    Wire.requestFrom(slave_2, 1);   
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
  
  if(received[6] == '1'){
    Serial.print("slave received: ");
    Serial.println(received);
    Serial.println(token);
    send = true;
  }
}

void requestEvent() {
  if(send){
    Wire.write(token);
    send = true;
  } 
}
