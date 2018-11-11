// include libraries
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>

byte inByte = 1;           // incoming byte from the SPI

#define deathpin 5
#define barpin 4
char spicommand[5];

//const char* ssid = "404";
//const char* password = "user not found";

// testing wifi
const char* ssid = "NETGEAR68";
const char* password = "jollymint969";


unsigned int remoteport=1390 ;      // local port to listen on
unsigned int localPort =  1808 ;      // local port to listen on

IPAddress remoteip(192, 168, 1, 64);
WiFiUDP Udp;

int color=1;
int value,r,g,b;
int leftint=5;
int rightint=5;
int weaponint=1;
int leftdata=0;
int rightdata=0;
char packetBuffer[255]; //buffer to hold incoming packet
char command_init[1];
char  ReplyBuffer[] = "141414141";       // a string to send back
char *pt;
char *sec;
float acc=0;
float til=0;
float ltilt=0;
float rtilt=0;
int motleft=0;
int motright=0;
int comman=0;
int deadswitch=0;

void setup() {
  setupSerial();
  pinsetup();
  setup_spi();
  setupudp();
   
}

void loop() {
  // put your main code here, to run repeatedly:
readdata(leftdata,rightdata);
send_spi_data();
}


// read incoming data from udp server on the phone
void readdata(int leftdata,int rightdata)
{
    int packetSize = Udp.parsePacket();
    if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    
    pt = strtok (packetBuffer,":");
    sec=pt;
    Serial.println("command ");
    Serial.println(sec);
    comman=atoi(sec);
    pt = strtok (NULL,":");
    value=atoi(pt);
    Serial.println("value ");
    Serial.println(value);
    if(comman==7){
      if(value==1){
      Serial.println("Red ");
        //colorWipe(strip.Color(255, 0, 0), 50); // Red
        color=1;
      }
      else if(value==2){
      Serial.println("blue");
//          colorWipe(strip.Color(0, 255, 0), 50); // blue
         color=2;

      }
    }else if(comman==8){
      if(value>90){
        digitalWrite(deathpin,HIGH);
      analogWrite(barpin,1024);
      }
      else if(value>80){
        digitalWrite(deathpin,HIGH);
        analogWrite(barpin,921);
      }
       else if(value>70){
        digitalWrite(deathpin,HIGH);
        analogWrite(barpin,819);
      }
      else if(value>60){
        digitalWrite(deathpin,HIGH);
        analogWrite(barpin,717);
      }
      else if(value>50){
        digitalWrite(deathpin,HIGH);
        Serial.println("health_here");
        analogWrite(barpin,615);
      }
      else if(value>40){
        digitalWrite(deathpin,HIGH);
        analogWrite(barpin,513);
      }
      else if(value>30){
        digitalWrite(deathpin,HIGH);
      analogWrite(barpin,411);
      }
      else if(value>20){
        digitalWrite(deathpin,HIGH);
      analogWrite(barpin,309);
      }
      else if(value>10){
        digitalWrite(deathpin,HIGH);
      analogWrite(barpin,207);
      }else if(value>0)
      {
        digitalWrite(deathpin,HIGH);
         analogWrite(barpin,105);
      }
      else if(value==0){
        leftint=5;
        rightint=5;
        Serial.println("here");
        digitalWrite(deathpin,LOW);
         analogWrite(barpin,0);
      }
    }else if(comman==1){
        leftint=value;
    }
    else if(comman==2){
        rightint=value;
    }
    else if(comman==3){
        weaponint=value;
    }
    else if(comman==4){
        weaponint=value;
    }
    
    
    }
 }
  
void pinsetup()
{
    pinMode(deathpin, OUTPUT);
}

void setupudp()
{
  
  WiFi.begin(ssid, password);// Connect to WiFi 
  WiFi.config(IPAddress(192, 168, 1, 64),
  IPAddress(192, 168, 1, 1),
  IPAddress(255, 255, 255, 0));
  
  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }
  //print a new line, then print WiFi connected and the IP address
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);

}

void setupSerial()
{
    Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}





void setup_spi(){
   digitalWrite(D3,HIGH);// put your setup code here, to run once:
  //pinMode(LED_BUILTIN,OUTPUT);
  pinMode(D3,OUTPUT);
  SPI.begin();
}

// send spi data to teensy
void send_spi_data(){
  
  byte c;
  SPI.beginTransaction (SPISettings(100000, MSBFIRST, SPI_MODE0));
  digitalWrite(D3, LOW);   // SS low (select slave)

  sprintf(spicommand, "%02d%02d%d\n",leftint,rightint,weaponint);
  //myString.toCharArray(spicommand, 6);
  for (int i=0; i<=5; i++)
     inByte=SPI.transfer(spicommand[i]);
     //Serial.print("hello");
     //Serial.println(inByte);
     digitalWrite(D3, HIGH);  // SS high (unselect slave)
     SPI.endTransaction();         // transaction over  
     // delay(10);
}


