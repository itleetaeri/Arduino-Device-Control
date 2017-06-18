/*Control Devices with SIM 900A & IR Remote
   LE TAN RI
   NGUYEN PHUC LOC
May 2017
*/

#include "SIM900.h" 
#include <SoftwareSerial.h> 
#include "sms.h"
#include "call.h"
#include <EEPROM.h>
#include "IRremote.h"
SMSGSM sms;
CallGSM call;
// For IR remote
int revPIN = 5;
IRrecv irrecv(revPIN);
decode_results results;

//Global variable for processing event
boolean started=false; //SIM Status
byte PWR=13; // Power led of SIM900A
enum STATUS {On, Off}; 
struct devices{
  byte Status;
  byte pin;
  int address;
  devices( byte stt, byte Pin, int add): Status(stt), pin(Pin), address(add) {};  
};
devices dev1(Off, 8, 1);
devices dev2(Off, 9, 2);
devices dev3(Off, 10, 3);
devices dev4(Off, 11, 4);

devices dev5(Off, 6, 5);
devices dev6(Off, 7, 6);
char lee_tae_ri[20]="+841288830408"; // Phone number in international format
char phuc_loc[20]="+84944281788";
char number[20];
//function
void callFunc();
void smsFunc();
void smsControl(char *command);
byte cmdCheck(char *command);
void setSTT(devices dev){
  if (dev.Status==On){
    digitalWrite(dev.pin, HIGH);
    return;
  }
   if (dev.Status==Off){
    digitalWrite(dev.pin, LOW);
    return;
  }
}
void readStatus();
void switchStt(devices &dev, char *command);
void printStatus(devices dev){//Print stt for debugging
  Serial.print(dev.address);
  Serial.print(" : ");
  Serial.println(dev.Status);
}

//IR remote function
void iRRecieve();
void setup(){  
    Serial.begin(9600);   
    pinMode(dev1.pin,OUTPUT);
    pinMode(dev2.pin,OUTPUT);
    pinMode(dev3.pin,OUTPUT);
    pinMode(dev4.pin,OUTPUT);
    pinMode(dev5.pin,OUTPUT);
    pinMode(dev6.pin,OUTPUT);
    pinMode(PWR,OUTPUT);
    
//    FirstUse: decoment this for initializing data.

//    EEPROM.update(dev1.address, Off);
//    EEPROM.update(dev2.address, Off);
//    EEPROM.update(dev3.address, Off);
//    EEPROM.update(dev4.address, Off);
//    EEPROM.update(dev5.address, Off);
//    EEPROM.update(dev6.address, Off);
    //Set all Devices off.
    //Get the last status of devices
    readStatus();
    Serial.println("Switcher");
    //Starting SIM900
    if (gsm.begin(2400)){     
        Serial.println("\nStatus=Online");
        digitalWrite (PWR,HIGH);//SIM is now on     
        started=true;     
    }   else 
        Serial.println("\nStatus=Offline");

    //  Turn on IR reciever
     irrecv.enableIRIn();
}
void loop() {
    if(started){
        callFunc();
        smsFunc();
        iRRecieve();
    }else Serial.println("Status: Offline");
    //Print out devices status on Serial Monitor, just for debugging
//    printStatus(dev0);
//    printStatus(dev1);
//    printStatus(dev2);
//    printStatus(dev3);
//    printStatus(dev4);
//    printStatus(dev5);
//    printStatus(dev6);
}

void callFunc(){
        byte call_Status=call.CallStatusWithAuth(number,1,4);
        if (call_Status==CALL_INCOM_VOICE_AUTH) {// If there is a call comming 
          Serial.println("Incoming call. Hang up in 3 second.");
        delay(2000);
        call.HangUp();
        }
        if (call_Status==CALL_INCOM_VOICE_NOT_AUTH) {
          call.HangUp();
          Serial.println(" Stranger is calling. Hung up.");
          sms.SendSMS(number,"BITCH!!!");
        }
}
void smsFunc(){
  char sms_check=sms.IsSMSPresent(SMS_ALL);
  if (sms_check){
    byte pos=(byte)sms_check;//Position of the unread sms.
    char present_num[20];
    char smstext[160];// SMS Detail
    char get_sms=sms.GetAuthorizedSMS(pos, present_num, smstext, 160,1,4);
    char delete_sms=sms.DeleteSMS(pos);
    Serial.println(smstext);
    //if (get_sms==GETSMS_AUTH_SMS){//Authoriezed SMS unread
      Serial.print("Number:  ");//from who
      Serial.println(present_num);
      Serial.println("Detail: ");// Print out SMS text
      Serial.println(smstext);
      byte passCheck=strstr(smstext,"Ri");// Password checking
      byte phone_call=strstr(smstext,"call");
      if(phone_call) {
        call.Call(lee_tae_ri);
        delay(30000);
        call.HangUp();
      }
       //Serial.println(passCheck);
      if (passCheck) smsControl(smstext); // If Authorized, goTo control function
      else Serial.println("Invail password.");
      byte sttCheck=strstr(smstext,"STATUS"); //Send the devices status to App
      if (sttCheck) { 
        String text;
        text += "STATUS ";
        text += "1x"; text +=dev1.Status; text += "x1 ";
        text += "2x"; text +=dev2.Status; text += "x2 ";
        text += "3x"; text +=dev3.Status; text += "x3 ";
        text += "4x"; text +=dev4.Status; text += "x4 ";
        text += "5x"; text +=dev5.Status; text += "x5 ";
        text += "6x"; text +=dev6.Status; text += "x6 ";
        //get Status ok, sending
        sms.SendSMS(lee_tae_ri, text.c_str());
        
      }
    //}
    if (delete_sms) Serial.println("Deleted");
  }
}
void smsControl(char *command){
  byte cmd=cmdCheck(command);
  switch (cmd){
    case 1:{
      switchStt(dev1,command);//Turn dev ON or OFF
      break;
    }
    case 2:{
      switchStt(dev2,command);//Turn dev ON or OFF
      break;
    }
    case 3:{
      switchStt(dev3,command);//Turn dev ON or OFF
      break;
    }
    case 4:{
      switchStt(dev4,command);//Turn dev ON or OFF
      break;
    }
    case 5:{
      switchStt(dev5,command);//Turn dev ON or OFF
      break;
    }
    case 6:{
      switchStt(dev6,command);//Turn dev ON or OFF
      break;
    }
    case -1:{
      switchStt(dev1,command);
      switchStt(dev2,command);
      switchStt(dev3,command);
      switchStt(dev4,command);
      switchStt(dev5,command);
      switchStt(dev6,command);
      break;
    }
    default:{
      Serial.println("No command found!");
    }
    
  }
  
}
byte cmdCheck(char *command){
  byte Dev1=strstr(command,"dev1");// find the taget devices
  byte Dev2=strstr(command,"dev2");
  byte Dev3=strstr(command,"dev3");
  byte Dev4=strstr(command,"dev4");
  byte Dev5=strstr(command,"dev5");
  byte Dev6=strstr(command,"dev6");
  byte All=strstr(command,"alldev");

  if (Dev1){
    Serial.println("Dev1");
    return dev1.address;
  }
  if (Dev2){
    Serial.println("Dev2");
    return dev2.address;
  }
  if (Dev3){
    Serial.println("Dev3");
    return dev3.address;
  } 
  if (Dev4){
    Serial.println("Dev4");
    return dev4.address;
  }
  if (Dev5){
    Serial.println("Dev5");
    return dev5.address;
  }
  if (Dev6){
    Serial.println("Dev6");
    return dev6.address;
  }
  if (All) {
    Serial.println("ALL DEVICES");
    return -1;
  }
  return -2;

}
void switchStt(devices &dev, char *command){
  // Change the status of devices
  byte ON=strstr(command, "on");//find instruction.
  byte OFF=strstr(command, "off");
  
  if ((ON!=0) && (OFF==0)){// if the device is now off, turn it on.
    dev.Status=On;
    setSTT(dev);//Switch Status
    Serial.println("Device is now ON");
    EEPROM.update(dev.address, dev.Status);//Save status to EEPROM
    return;
  }
  if ((ON==0) && (OFF!=0)){//otherwise, turn it off.
    dev.Status=Off;
    setSTT(dev);
    Serial.println("Device is now OFF");
    EEPROM.update(dev.address, dev.Status);//Save status to EEPROM
    return;
  }
  else {
    Serial.println("Invail");
  }
}


void iRRecieve()
{
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    if (results.value == 0xFF30CF){
       if (dev1.Status == On) switchStt(dev1,"off");
       else switchStt(dev1,"on");
    }
    if (results.value == 0xFF18E7){
       if (dev2.Status == On) switchStt(dev2,"off");
       else switchStt(dev2,"on");
    }
     if (results.value == 0xFF7A85){
       if (dev3.Status == On) switchStt(dev3,"off");
       else switchStt(dev3,"on");
    }

     if (results.value == 0xFF10EF){
       if (dev4.Status == On) switchStt(dev4,"off");
       else switchStt(dev4,"on");
    }
     if (results.value == 0xFF38C7){
       if (dev5.Status == On) switchStt(dev5,"off");
       else switchStt(dev5,"on");
    }
     if (results.value == 0xFF5AA5){
       if (dev6.Status == On) switchStt(dev6,"off");
       else switchStt(dev6,"on");
    }

    //Call Ri
    if (results.value == 0xFF629D)
      call.Call(lee_tae_ri);
    if (results.value == 0xFFA25D)
      call.HangUp();
    //Call Loc
     if (results.value == 0xFFE21D)
      call.Call(phuc_loc);
    irrecv.resume();
 }
}


void readStatus() 
{
    dev1.Status=EEPROM.read(dev1.address);//Read devices status from EEPROM
    dev2.Status=EEPROM.read(dev2.address);
    dev3.Status=EEPROM.read(dev3.address);
    dev4.Status=EEPROM.read(dev4.address);
    dev5.Status=EEPROM.read(dev5.address);
    dev6.Status=EEPROM.read(dev6.address);
    String text;
    text += "STATUS ";
    text += "Dev 1: "; text +=dev1.Status; text += " ";
    text += "Dev 2: "; text +=dev2.Status; text += " ";
    text += "Dev 3: "; text +=dev3.Status; text += " ";
    text += "Dev 4: "; text +=dev4.Status; text += " ";
    text += "Dev 5: "; text +=dev5.Status; text += " ";
    text += "Dev 6: "; text +=dev6.Status; text += " ";
    Serial.println(text.c_str());
    setSTT(dev1);
    setSTT(dev2);
    setSTT(dev3);
    setSTT(dev4);
    setSTT(dev5);
    setSTT(dev6);
}

