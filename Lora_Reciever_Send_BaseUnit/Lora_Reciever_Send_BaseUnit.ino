/*Set following parameters in lora module using USB to TTL convertor for base unit
AT+IPR=115200
AT+ADDRESS=1
AT+NETWORKID=5
AT+MODE=0
AT+BAND=915000000
AT+PARAMETER=12,7,1,4
*/
int incomingByte;

void setup() {
Serial.begin(115200);
delay(20);
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

void loop() {
          if(Serial.available()>0){
            String incomingByte = Serial.readString();
            if(incomingByte.startsWith("+RCV")){
              String payload = getPayload(incomingByte);
              Serial.print("AT+SEND=2," + String(payload.length()) + "," + payload + "\r\n");
            }
          }
}
