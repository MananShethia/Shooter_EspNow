#include <EspNow.h>
#include <positionalPid.h>

Peer myEsp;
json feedback;
bool mtpos = false;
bool  mtfix = false;
bool mtreset = false;
bool defaultflag = false;
bool defaultflag2 = false;
bool resetflag = false;
#define limitSwitch 23

motor m1(2, 15, 27, 12, 14, 9211, -1);
positional pos1(&m1);
int angle=20000;
int val;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(5);
  setId("MT");//MAX 6 chars This is its own id
  myEsp.init("Shoot");  //  register peer with peer_id
  myEsp.setOnRecieve(defaultHandler);

  pinMode(limitSwitch, INPUT);
  ////////////////////////
  pos1.setThreshold(300);
  pos1.setAggTunings(1.09, 0, 0);
  pos1.setSoftTunings(0.2, 0, 0);
  ////////////////////////

  // feedback.addUnit("feedback", "1");
  feedback.addUnit("MegaTorqueFBK", "lastpos");
  //feedback.addUnit("MegaTorqueFBK", "fixpos");
}

int reading;
int sc;
void loop()
{
  pos1.compute();
  reading=m1.getReadings();
  Serial.println(reading);
  if (digitalRead(limitSwitch) == HIGH && mtpos == true ) {

    //Serial.println("loop 1");

    myEsp.send(feedback);
    Serial.println("fbk sended 1");
    mtpos = false;
    //delay(2000);
    
  
  }
   else if ( digitalRead(limitSwitch) == LOW && mtfix == true)
  {
    //feedback.clear();
    //Serial.println("loop 2");
    sc= m1.getReadings();
    pos1.setPulse(sc);
    
    feedback.updateValue("MegaTorqueFBK", "fixpos");
    myEsp.send(feedback);
    Serial.println("fbk sended 2");
    mtfix = false;
  }
  else if(mtreset == true)
  {
      feedback.addUnit("ResetMegaTorqueFBK", "mt_reset");
      myEsp.send(feedback);
      Serial.println("Reset sended");
      mtreset = false;
  }

}


void defaultHandler(json msg)
{
  if (String(msg.getValue("MegaTorque")) == "start" && defaultflag == false)
  {
   // Serial.println("START");
    MTlast();

    defaultflag = true;

    // feedback.clear();
    //msg.clear();
  }

  else if (String(msg.getNumberValue("MegaTorqueFix")) && defaultflag2 == false)
  {
    //Serial.println("FIX");
    //Serial.println(String(msg.getNumberValue("MegaTorque")));
    val = msg.getNumberValue("MegaTorqueFix");
    Serial.println(val);
//    Serial.println(val);
    MTfix(val);
    
    //feedback.clear();
    defaultflag2 = true;
  }
  else if(String(msg.getValue("Reset")) == "reset" && resetflag == false)
  {
      MTreset()
      resetflag = true;
  }
}

void MTreset()
{
    angle = 0;
    pos1.setPulse(angle);
    mtreset = true;
}

void MTlast()
{
  //Serial.println("MTLAST");
  pos1.setPulse(angle);
  mtpos = true;
}

void MTfix(int pc)
{

  //Serial.println(pc);
  pos1.setPulse(pc);
  mtfix = true;
  Serial.println("mtfix");
}