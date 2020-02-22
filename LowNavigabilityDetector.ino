#include <SoftwareSerial.h>
#include <TinyGPS++.h>
SoftwareSerial GPS_Serial(2,3);
SoftwareSerial GSM_Serial(4,5);
TinyGPSPlus gps;
const int trig = 6;
const int echo = 7;
int buzzer = 8;
int LED = 9;
int duration;
double distance;
double avgDistance=0;
String Distance = "";
String info;
String Location;
String Location1="";
String Location2="";
int count=1;
void setup() {
  // put your setup code here, to run once:
  pinMode(trig, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
  GPS_Serial.begin(9600);
  GSM_Serial.begin(9600);
  Serial.println("Date, Time, Latitude, Longitude, Depth(m)");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(buzzer, LOW);
  digitalWrite(LED, LOW);
  delay(100);
  GPSinfo();
  measure();
  while(distance < 4)
  { 
    avgDistance = (avgDistance+distance)/count;
    GPSinfo();
    if(Location1=="")
    {
      Location1+=Location;
    }
    alarm();
    SMS("Measured distance"+String(distance)+"m");
    measure();
    count++;
  }
  if(Location2=="")
    {
      Location2+=Location;
    }
  if(Location1!="" && Location2!="")
    {
      SMS(message(Location1, Location2, String(avgDistance)));
    }
}
void alarm()
{
   digitalWrite(buzzer, HIGH);
   digitalWrite(LED, HIGH);
   delay(1000);
   digitalWrite(buzzer, LOW);
   digitalWrite(LED, LOW);
}
double measure()
{
   digitalWrite(trig, LOW);
   delay(2);
   digitalWrite(trig, HIGH);
   delay(10);
   digitalWrite(trig, LOW);
   duration = pulseIn(echo, HIGH);
   distance = duration*0.0015/2;
   Serial.println(distance);
   return distance;
}
void GPSinfo()
{
  if(GPS_Serial.available())
  {
    gps.encode(GPS_Serial.read()); 
  }
    
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.year());
    Serial.print(", ");
    
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.print(gps.time.second());
    Serial.print(", ");
    
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
   
    Serial.print(gps.location.lng(), 6);
    Serial.print(", ");
    String Location = String(gps.location.lat(), 6)+ String(gps.location.lng(), 6);
}
void SMS(String s)
{
  GSM_Serial.println("AT+CMGF=1");
  delay(1000);
  GSM_Serial.println("AT+CMGS=\"+8801957760475\"\r");
  delay(1000);
  GSM_Serial.println(s);
  delay(1000);
  GSM_Serial.println((char)26);
  delay(1000);
}
String message(String a, String b, String c)
{
  String s = "Low navigability found from latitude, longitude "+a+" to latitude, longitude"+b+", average depth "+c+" .";
  return s;
}
