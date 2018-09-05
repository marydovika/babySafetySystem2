#include <LiquidCrystal.h>
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
#include <VirtualWire.h>
String error_msg = "";
const int RCVD_INDICATOR = 5;
const int BUZZER = 6;
const int FAULTY = 2, IN_RANGE = 1, OFF_RANGE = 0; 
int last_state = -1;


void initVW(byte pin, long rate)
{
  // Initialise the IO and ISR
   vw_set_ptt_inverted(true); // Required for DR3100
   vw_setup(rate);   // Bits per sec
   vw_set_rx_pin(pin);
   vw_rx_start();       // Start the receiver PLL running
}
String readVW()
{
  String ret = "";
  uint8_t buf[VW_MAX_MESSAGE_LEN];
   uint8_t buflen = VW_MAX_MESSAGE_LEN;
   if (vw_get_message(buf, &buflen)) // Non-blocking
   {  
     for (int i = 0; i < buflen; i++)
      {
        ret = ret + (char)buf[i] ;//lcd.print((char)buf[i]);
      }
  }
  while( vw_get_message(buf, &buflen) ){} //read remaining bytes
  ret.trim();
  if( ret.startsWith("BABY ") ){  ret = ret.substring(5, ret.length() ); }
  else{ ret=""; }
  return ret;
}

void ignoreResponse()
{
  while(!Serial.available()){} delay(1000);
  while(Serial.available()){Serial.read();}
}
boolean initModem()
{
  Serial.begin(19200); delay(500);
  Serial.println("AT"); delay(500);  
  String tt=""; while(Serial.available()){ tt = tt + (char)Serial.read(); }
  if( !(tt.indexOf("OK")>-1) ){ initModem(); }
  else
  {    
    delay(3000);
    Serial.println("AT+IPR=19200");  delay(500);
    Serial.println("AT+CSMP=17,167,0,16\r"); 
    ignoreResponse();
  }
}

void send_sms()
{ 
  
  //send tpo second number
    Serial.println("\r");delay(500);
    while(Serial.available()){ Serial.read();}
    Serial.println("AT+CMGF=1\r"); delay(500);
    Serial.println("AT+CMGS=\"+256753215106\"\r");
    delay(500);
    Serial.print(error_msg);
    delay(500); Serial.println("\r"); delay(500);
    Serial.write(26); ignoreResponse();
}

void setup()
{
   lcd.begin(16, 2);
   lcd.print("BABY SAFETY");
   pinMode(RCVD_INDICATOR,OUTPUT);
   digitalWrite(RCVD_INDICATOR, LOW);
   pinMode(BUZZER,OUTPUT);
   digitalWrite(BUZZER, LOW);
   delay(1000);
   initVW(9, 4000);
   delay(1000);

  lcd.clear(); lcd.print("Network Search");
  initModem(); //wait for _gsm to power up
  lcd.clear(); lcd.print("System ON");
}
int get_state()
{
  error_msg = "";
  long startT = millis();
  while(  (millis()-startT)<1500 )//check for 3 sec
  {
     String val = readVW();
     if(val!="")
     {
       digitalWrite(RCVD_INDICATOR, HIGH);
       delay(50);
       digitalWrite(RCVD_INDICATOR, LOW);
       
       if(val.startsWith("NEAR") || val.startsWith("FAR") ){
          return IN_RANGE;
       }
       else if(val.startsWith("BELT") || val.startsWith("POWER") ){ 
        error_msg = val;
        return FAULTY;
      }
     }
  }
  return OFF_RANGE;
}
void loop()
{
   int state = get_state();
   if(last_state!=state){
     lcd.clear(); lcd.print("Baby Status:");
     lcd.setCursor(0,1); 
     if(state==FAULTY)
     {
        digitalWrite(BUZZER, HIGH);
        lcd.print(error_msg); 
        send_sms();
     }
     else if(state==IN_RANGE)
     {
        digitalWrite(BUZZER, LOW);
        error_msg = "IN RANGE";
        lcd.print(error_msg);
        send_sms();
     }
     else if(state==OFF_RANGE){
        digitalWrite(BUZZER, HIGH);
        error_msg = "OUT OF RANGE";
        lcd.print(error_msg);
        send_sms();
     }
   }
   delay(1000);
   last_state=state;
}

