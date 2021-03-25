#include <EspNow.h>
#include<ESP32Servo.h>

Peer myEsp;
json feedback;

Servo lockServo1;
Servo lockServo2;

void setup() {
  Serial.begin(115200);
  setId("Lock");//MAX 6 chars This is its own id
  myEsp.init("Shoot");  //  register peer with peer_id
  myEsp.setOnRecieve(defaultHandler);
  
  lockServo1.attach(19);
  lockServo2.attach(21);
  
  lockServo1.write(0);
  lockServo2.write(180);
  
  feedback.addUnit("feedback", "1");
}

void loop() {
}



void defaultHandler(json msg) 
{
  if (String(msg.getValue("Locking")) == "lock")
  {
    lockServoRoutine(110, 80); 
    feedback.addUnit("LockFBK", "rubberlock");
    myEsp.send(feedback);
  }
  else if (String(msg.getValue("Releasing")) == "release") 
  {
    lockServoRoutine(0, 180);
    feedback.addUnit("ReleaseFBK", "rubberrelease");
    myEsp.send(feedback);
  }
  else if(String(msg.getValue("Reset")) == "reset")
  {
    lockServoRoutine(90,90)
    feedback.addUnit("ResetLockServoFBK", "ls_reset");
    myEsp.send(feedback);
    Serial.println("Reset sended");
  }
}

void lockServoRoutine(int pos1, int pos2) 
{
  lockServo1.write(pos1);
  lockServo2.write(pos2);
  Serial.println("lock sended");
}