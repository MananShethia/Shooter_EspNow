#include <EspNow.h>
#include<ESP32Servo.h>

Peer myEsp;
json feedback;

Servo gateServo;
Servo sepServo3;
Servo sepServo4;

void setup() {
  Serial.begin(115200);
  setId("Funnel");//MAX 6 chars This is its own id
  myEsp.init("Shoot");  //  register peer with peer_id
  myEsp.setOnRecieve(defaultHandler);
  gateServo.attach(5);
  sepServo3.attach(19);
  sepServo4.attach(21);
  gateServo.write(90);
  sepServo3.write(90);
  sepServo4.write(90);
  
  feedback.addUnit("feedback", "1");
  pinMode(33, OUTPUT);
  // feedback.addValue("PnumeticFBK","pnumeticon");
  //feedback.addValue("SeprateFBK","seprated");
}

void loop() {
}



void defaultHandler(json msg) 
{
  if (String(msg.getValue("Pnumetic")) == "on" && String(msg.getValue("GateServo")) == "off")
  {
    gateServoRoutine(180);
    pnumeticRoutine(0); 
    myEsp.send(feedback);
  }
  else if (String(msg.getValue("SepServo")) == "on") 
  {
    feedback.clear();
    sepServoRoutine(1, 180);
    myEsp.send(feedback);
  }
  else if (String(msg.getValue("Pnumetic")) == "off") 
  {
    feedback.clear();
    pnumeticRoutine(1);
    myEsp.send(feedback);
  }
  else if(String(msg.getValue("Reset")) == "reset")
  {
      feedback.clear();
      pnumeticRoutine(0);
      gateServoRoutine(90);
      sepServoRoutine(90, 90);
      feedback.addUnit("ResetGateFBK", "gate_reset");
      feedback.send(feedback);
      Serial.println("Reset sended");
  }
}

void gateServoRoutine(int pos) 
{
  gateServo.write(pos);
  feedback.addUnit("GateFBK", "gateoff");
  Serial.println("gate off sended");
}
void sepServoRoutine(int sep1, int sep2) 
{
  sepServo3.write(sep1);
  sepServo4.write(sep2);
  delay(2000);
  feedback.addUnit("SepServoFBK", "sepon");
  Serial.println("Separate servo FBK sended");
}
void pnumeticRoutine(int state) 
{
  if (state == 0) 
  {
    digitalWrite(33, state);
    delay(1000);
    feedback.addUnit("PnumeticFBK", "pnumeticon");
    Serial.println("pnumetic on sended");
  }
  else if (state == 1) 
  {
    digitalWrite(33, state);
    delay(1000);
    digitalWrite(33, 0);
    delay(1000);
    feedback.addUnit("PnumeticFBK", "pnumeticon");
    Serial.println("pnumetic off on sended");
  }
}
