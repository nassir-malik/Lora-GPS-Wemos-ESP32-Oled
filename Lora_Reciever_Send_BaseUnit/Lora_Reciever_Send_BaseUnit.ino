
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
