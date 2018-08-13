/*Set following parameter in lora module using USB to TTL convertor
AT+IPR=115200
AT+ADDRESS=2
AT+NETWORKID=5
AT+MODE=0
AT+BAND=915000000
AT+PARAMETER=12,7,1,4
*/


#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <TinyGPS++.h>                                  // Tiny GPS Plus Library

#define SERIAL1_RXPIN 12 //to GPS TX
#define SERIAL1_TXPIN 13 //to GPS RX

#define SERIAL2_RXPIN 2 //to Lora TX
#define SERIAL2_TXPIN 14 // to Lora RX

HardwareSerial Serial1(1);
HardwareSerial Serial2(2);
const float Home_LAT = 33.205288;                      // Your Home Latitude
const float Home_LNG =  -96.951125;                     // Your Home Longitude
int incomingByte;
TinyGPSPlus gps;                                        // Create an Instance of the TinyGPS++ object called gps
String loraResponse;
boolean responsePending;
unsigned long currentTime;
unsigned long loopTime;

SSD1306Wire  display(0x3c, 5, 4);
int counter=0;
void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN); // GPS com
  Serial2.begin(115200, SERIAL_8N1, SERIAL2_RXPIN, SERIAL2_TXPIN); // Lora com
  
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}



void loop() {

  // clear the display
   display.clear();
   //drawFontFaceDemo();

   display.setTextAlignment(TEXT_ALIGN_LEFT);
   
   /* Latitude */
   display.setFont(ArialMT_Plain_10);
   display.drawString(0, 0, "Latitude: " + String(gps.location.lat(),4));
   
   /* Longitude */
   display.setFont(ArialMT_Plain_10);
   display.drawString(0, 9, "Longitude: " + String(gps.location.lng(),4));

   /* Satellites */
   display.setFont(ArialMT_Plain_10);
   display.drawString(0, 19, "Sat: " + String(gps.satellites.value()));
   
   /* Distance */
   display.setFont(ArialMT_Plain_10);
   //display.drawString(40, 19, "Sp: " + String(gps.speed.mph()));
   unsigned long distanceKmToHomw =
   (unsigned long)TinyGPSPlus::distanceBetween(
     gps.location.lat(),
     gps.location.lng(),
     Home_LAT, 
     Home_LNG) / 1000;
    display.drawString(40, 19, "Distance: " + String(distanceKmToHomw,4));
    
    /* Heading*/
   display.setFont(ArialMT_Plain_10);
   display.drawString(0, 29, "Heading: " + String(gps.course.deg()));
   
   //display.setFont(ArialMT_Plain_10);
   //display.drawString(0, 39, "Elevation: " + String(gps.altitude.feet()));
   
   String timestr = String(gps.time.hour()) + ":" + String(gps.time.minute())+ ":" + String(gps.time.second());
   display.setFont(ArialMT_Plain_16);
   //display.drawString(0, 49, "Time UTC: " + timestr);
   display.drawString(0, 45, "Time: " + getPayload(loraResponse));
    //Serial.println(timestr);
  // write the buffer to the display
  display.display();
  counter++;
  delay(1000);
  smartDelay(500);



  
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
  //end message to lora module anf get response
  
  Serial.println("Sending lora message...");
  Serial2.print("AT+SEND=1," + String(timestr.length())+ "," + timestr + "\r\n");
  responsePending = true;
  currentTime = millis();
  while(responsePending){
    loopTime = millis(); //5 sec wait for lora response
      if(Serial2.available()>0){
          loraResponse = Serial2.readString();
            if(loraResponse.startsWith("+RCV")  ){
              Serial.println("Got response data...");
              Serial.println(loraResponse);
              responsePending = false;
              break;
            }
      }
      if(loopTime>=currentTime+5000){loraResponse ="Disconnted";break;} //bailout if no response from lora
      delay(20);
  }
  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
}

static void smartDelay(unsigned long ms)                // This custom version of delay() ensures that the gps object is being "fed".
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())
    
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}


String getPayload(String data){
  int lastCommaIndex = data.lastIndexOf(',');
  data.remove(lastCommaIndex );
  lastCommaIndex = data.lastIndexOf(',');
  data.remove(lastCommaIndex );
  lastCommaIndex = data.lastIndexOf(',');
  String payload = data.substring(lastCommaIndex+1); 
  return payload ;
}

