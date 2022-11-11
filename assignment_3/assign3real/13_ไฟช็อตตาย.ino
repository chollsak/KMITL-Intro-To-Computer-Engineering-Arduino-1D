#define button1 2
#define button2 3
int buttonState1 = HIGH;
int buttonState2 = HIGH;
int bitPattern[13] = {
  B01111111,
  B00001101,
  B10110111,
  B10011111,
  B11001101,
  B11011011,
  B11111011,
  B00001111,
  B11111111,
  B11011111,
  B01110001,
  B01111011,
  B01111111
};
int ans;
const byte numPins = 8;
const int segmentPins[8] = {6, 7, 8, 9, 10, 11, 12, 13};
int lastButtonState[2] = {HIGH, HIGH};
int buttonState[2] = {HIGH, HIGH};
int button[2] = {button1, button2};
int reading[2];
long long lastDebounceTime[2] = {0, 0};
int Delay = 50;
int num = 1;
long long coolTime[2] = {0, 0};
int tmp;

void setup () {
  Serial.begin(9600);
  for (int i = 0; i < numPins; i++)
        pinMode(segmentPins[i], OUTPUT);
  
    pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
  
    randomSeed(analogRead(A0));
    ans = random(1, 10);
    Serial.print("ans = ");
    Serial.println(ans);
  
    Show(1);
    //Serial.println(ans);
}
void randomAns(){
    ans = random(1, 10);
    Serial.print("ans = ");
    Serial.println(ans);
}
void debounce(int it){
  
    reading[it] = digitalRead(button[it]);
    //Serial.println(reading);
    if(lastButtonState[it] != reading[it]){
        lastDebounceTime[it] = millis();
        //Serial.println("pressed button A");
    }
  
    if(millis() - lastDebounceTime[it] > Delay){
        buttonState[it] = reading[it];
    }
  
    lastButtonState[it] = reading[it];
}
  
void Show(int number){
  if(number < 10)
  Serial.println(number);
  for(int i = 0; i < 8; i++){
    int isBitSet = bitRead(bitPattern[number], i);
    digitalWrite(segmentPins[i], isBitSet);
    Serial.println(isBitSet);
  }
}

void pressedA(){
    if(millis() - coolTime[0] > Delay){
      num = (num % 9) + 1;
      Show(num);
    }
  
    coolTime[0] = millis();
}

void pressedB(){
  if(millis() - coolTime[1] > Delay){
    if(num < ans){
        tmp = 10;
        Serial.println("Less Than");
     }
     else if(num > ans){
         tmp = 11;
         Serial.println("Greater Than");
     }
     else{
         tmp = 12;
        Serial.println("Equal");
        randomAns();    
     }
    Show(tmp);
  }
  
  coolTime[1] = millis();
}

void loop() {
    //int reading = digitalRead(button1);
    debounce(0);
    debounce(1);
    
  //Serial.print("ans ======== ");
    //Serial.println(ans);
  
  if(buttonState[0] == LOW){
    //Serial.println("AAAAAAAAAAA");
    pressedA();
  }
  if(buttonState[1] == LOW){
    //Serial.println("BBBBBBBBBBB");
    pressedB();
  }
}