#include <EspNow.h>
Peer myEsp;
Peer MtEsp;
Peer LockEsp;
#define IRpin 23
json data;
json MtData;
json LockData;
json reset;
// #define IRpin 23
// int state;
// int count = 0;
// int laststate = HIGH;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  setId("Shoot");//MAX 6 chars This is its own id
  myEsp.init("Funnel");            //register peer with peer_id
  MtEsp.init("MT"); 
  LockEsp.init("Lock");           
  myEsp.setOnRecieve(your_function); //recevies data from other peer
  MtEsp.setOnRecieve(mt_function); //recevies data from other peer
  LockEsp.setOnRecieve(lock_function); //recevies data from other peer
  pinMode(23,INPUT);
//   pinMode(IRpin, INPUT);

  data.clear();
  MtData.clear();
  LcokData.clear();
  data.addUnit("GateServo", "on");
  MtData.addUnit("initial", "start");
}

String Serialinput;
String pulsecount;
void loop()
// {
// //   state = digitalRead(ir);
// //   if (state != lastcount)
// //   {
// //     //Serial.println("\tOBJECT DETECTED");
// //     count++;
// //     if(count%2 == 0)
// //     {
// //       Serial.println(count/2);
// //     }
// //   }
// //   lastcount = state;

  
  if (Serial.available())
  {
    Serialinput = Serial.readStringUntil(',');
    pulsecount = Serial.readStringUntil('\n');
    
    if (Serialinput == "gon")
    {
      data.clear();
      data.addUnit("Pnumetic", "on");
      data.updateValue("GateServo", "off");
      myEsp.send(data);
    }
    if(Serialinput == "reset")
    {
        reset.clear();
        reset.addUnit("Reset", "reset");
        myEsp.send(reset);
        // MtEsp.send(reset);
        LockEsp.send(reset);
    }
   
  }
}

int val;
bool mtflag = false;
bool lockflag = false;

void your_function(json msg)
{
  if(String(msg.getValue("GateFBK")) == "gateoff" && String(msg.getValue("PnumeticFBK")) == "pnumeticon")
  {
    Serial.println(">>>GATE OPENED");
    msg.addUnit("SepServo", "on");
    //delay(2000);
    myEsp.send(msg);

    LockData.clear();
    LockData.addUnit("Locking", "lock");
    LockEsp.send(LockData);
    
    if(lockflag == true)
    {
        Serial.println(">>>RUBBER LOCKED");
        MtData.clear();
        MtData.addUnit("MegaTorque", "start");
        MtEsp.send(MtData);
        lockflag = false;
    }
  }
  else if(String(msg.getValue("SepServoFBK")) == "sepon")
  {
    Serial.println(">>>ARROW SEPARATED");
    msg.addUnit("Pnumetic", "off");
    //delay(1500);
    myEsp.send(msg);
  }
  else if(String(msg.getValue("PnumeticFBK")) == "pnumeticon")
  {
    Serial.println(">>>PNUMETIC OFF ON"); 
    Serial.println(">>>ARROW OUT");
    if(mtflag == true && dgitalRead(ir)==LOW)
    {
      val = pulsecount.toInt();
      Serial.println(val);
      MtData.addUnit("MegaTorqueFix", val);
      MtEsp.send(MtData); 
      Serial.println(">>>PULSECOUNT SENDED");
      mtflag = false
    }
  }
  else if(String(msg.getValue("ResetGateFBK")) == "gate_reset")
  {
    Serial.println(">>>GATE RESET");
  }
}

void mt_function(json mtmsg)
{
  if(String(mtmsg.getValue("MegaTorqueFBK")) == "lastpos")
  {
    Serial.println(">>>MT AT LAST POSITION");
    mtflag = true;
  }
  else if(String(mtmsg.getValue("MegaTorqueFBK")) == "fixpos")
  {
    Serial.println(">>>MT AT FIX POSITION");

    LockData.addUnit("Releasing", "release");
    LockEsp.send(LockData);
    Serial.println("RELEASING SENDED");
  }
  else if(String(msg.getValue("ResetMegaTorqueFBK")) == "mt_reset")
  {
    Serial.println(">>>MEGA TORQUE RESET");
  }
}

void lock_function(json msg)
{
  if(String(msg.getValue("LockFBK")) == "rubberlock")
  {
    Serial.println(">>>RUBBER LOCKED");
    lockflag = true;
  }
  else if(String(msg.getValue("ReleaseFBK")) == "release")
  {
    Serial.println(">>>RUBBER relesed");
  }
  else if(String(msg.getValue("ResetLockServoFBK")) == "ls_reset")
  {
    Serial.println(">>>LOCK SERVO RESET");
    MtEsp.send(reset);
  }
}