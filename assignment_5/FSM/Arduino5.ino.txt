#define LED_P_G 13
#define LED_P_R 12
#define PEOPLE_BUTTON_PIN 5

#define LED_S_G 9
#define LED_S_Y 10
#define LED_S_R 11
#define SOUTH_BUTTON_PIN 4

#define LED_W_R 6
#define LED_W_Y 7
#define LED_W_G 8
#define WEST_BUTTON_PIN 3

#define goW 0
#define waitW 1
#define goS 2
#define waitS 3
#define walk 4
#define waitWalk 5
int nextstate;
struct state {
  unsigned long ST_Out;
  unsigned long Time;
  unsigned long Next[8];
};

typedef const struct state Stype;

Stype FSM[20] = {
  {B10110011,2000,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}}, // goW
  {B10110101,300,{goS,goS,goS,goS,walk,walk,goS,goS}}, // 1 waitW
  {B10011110,2000,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS}}, // goS
  {B10101110,300,{goW,goW,goW,goW,walk,goW,walk,walk}}, // 3 waitS
  {B01110110,2000,{walk,waitWalk,waitWalk,waitWalk,walk,waitWalk,waitWalk,waitWalk}}, // walk
  {B11110110,300,{6,6,6,6,6,6,6,6}}, // 5 waitWalk
  {B01110110,300,{7,7,7,7,7,7,7,7}}, // 6
  {B11110110,300,{8,8,8,8,8,8,8,8}}, // 7
  {B01110110,300,{9,9,9,9,9,9,9,9}}, // 8
  {B11110110,300,{10,10,10,10,10,10,10,10}}, // 9
  {B01110110,300,{10,goW,goS,goW,10,goW,goS,goW}}, // 10
  };
  
unsigned long S=0; 

void setup() {
  Serial.begin(9600);
  pinMode(LED_P_R, OUTPUT);
  pinMode(LED_P_G, OUTPUT);  
  pinMode(PEOPLE_BUTTON_PIN, INPUT);
  pinMode(LED_W_R, OUTPUT);
  pinMode(LED_W_Y, OUTPUT);
  pinMode(LED_W_G, OUTPUT);  
  pinMode(WEST_BUTTON_PIN, INPUT);  
  pinMode(LED_S_R, OUTPUT);
  pinMode(LED_S_Y, OUTPUT);
  pinMode(LED_S_G, OUTPUT);  
  pinMode(SOUTH_BUTTON_PIN, INPUT);
}

int input,inputW,inputS,inputP;

void loop() {
  digitalWrite(LED_W_G, FSM[S].ST_Out& B00000001);
  digitalWrite(LED_W_Y, FSM[S].ST_Out& B00000010);
  digitalWrite(LED_W_R, FSM[S].ST_Out& B00000100);

  digitalWrite(LED_S_R, FSM[S].ST_Out& B00001000);
  digitalWrite(LED_S_Y, FSM[S].ST_Out& B00010000);
  digitalWrite(LED_S_G, FSM[S].ST_Out& B00100000);

  digitalWrite(LED_P_R, FSM[S].ST_Out& B01000000);
  digitalWrite(LED_P_G, FSM[S].ST_Out& B10000000);

  delay(FSM[S].Time);

  inputW = digitalRead(WEST_BUTTON_PIN);
  inputS = digitalRead(SOUTH_BUTTON_PIN);
  inputP = digitalRead(PEOPLE_BUTTON_PIN);

  input = inputP*4+inputS*2+inputW;
  Serial.println(input);
//  if(S == 1|| S == 3||S == 10){
//    S = nextstate;
//  }
//  else{
//    if(S==0||S==2||S==4){
//      nextstate = FSM[FSM[S].Next[input]].Next[input]; // แสดง state ถัดไปอีก 1 state
//      if(S == 4){
//        nextstate = FSM[10].Next[input]; 
//      }
//    }
//    
    S = FSM[S].Next[input];
//  }

}
