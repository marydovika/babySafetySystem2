//simple Tx on pin D12
#include <VirtualWire.h>
  //char *controller;
const int BELT = 2; //whether belt is still connected
const int PWR = 3;//whether system is still powered
const int RED = A1;//red led AND buzzer, IN CATASTROPHE
const int GREEN = A0;//GREED LED on ehen state os OK
const int PWR12 = 10;//Pin that enables high power transmission

void initVWrireTx(byte pin, long rate)
{
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(pin);//pin where transmitter is connected
  vw_setup(rate);// speed of data transfer Kbps
}
void sendVW(char *txt)
{
  vw_send((uint8_t *)txt, strlen(txt));
   vw_wait_tx(); // Wait until the whole message is gone
  
}
void myDelay(long tym)
{
  tym /= 10;
  while(tym--)
  {
    delay(10);
    if((digitalRead(BELT)==HIGH )&&(digitalRead(PWR)==LOW ))
    {
      sendVW("BABY BELT&PWR OFF");
    }
    else if(digitalRead(BELT)==HIGH ){ sendVW("BABY BELT OFF");  }
    else if(digitalRead(PWR)==LOW ){ sendVW("BABY POWER OFF"); }
    if((digitalRead(BELT)==HIGH )||(digitalRead(PWR)==LOW ))
    {
      digitalWrite(RED, HIGH);
      digitalWrite(GREEN, LOW);
    }
    else
    {
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, HIGH);
    }
  }
}
void setup() 
{
  pinMode(BELT, INPUT_PULLUP);
  pinMode(PWR, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(PWR12, OUTPUT);
  digitalWrite(PWR12, LOW);
      digitalWrite(RED, HIGH);
      delay(200);
      digitalWrite(GREEN, HIGH);
      delay(200);
      digitalWrite(RED, LOW);
  initVWrireTx(9, 4000);
}

void loop()
{
  //digitalWrite(PWR12, LOW);
  myDelay(1000);
  sendVW("BABY NEAR");
  //digitalWrite(PWR12, HIGH);
  //myDelay(500);
  //sendVW("BABY FAR");

}
