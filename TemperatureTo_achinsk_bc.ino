#include <stdlib.h>

#include <Wire.h>

#include <OneWire.h>
// DS18S20 Temperature chip i/o
OneWire  oneWire(8); // on pin 10

#include <DallasTemperature.h>
DallasTemperature sensors(&oneWire);

#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int led = 0;

#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,0,254);

IPAddress gateway(192,168,0,1);

// fill in your Domain Name Server address here:
IPAddress myDns(8,8,8,8);

// initialize the library instance:
EthernetClient client;

char server[] = "APPENGINEID.appspot.com";

unsigned int nextTime = 0;          // next attempt connect to the server, in times
unsigned int postTimesAttempts;    //  post times to server use in ( uptime / postingInterval )
boolean lastConnected = false;                 // state of the connection last time through the main loop
// 10 minutes
const unsigned long postingInterval = 1800000;// delay between updates, in milliseconds

void setup() {                
  Serial.begin(9600);  
  Wire.begin();
  
  // Init temperature sensors
  sensors.begin();
  
  lcd.begin(16, 2);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  
  Ethernet.begin(mac, ip, myDns, gateway);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

}

char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}

char buffer [100];
char t1 [8];
char t2 [8];
char t3 [8];
char t4 [8];

char * httpStatus = "U";

// this method makes a HTTP connection to the server:
void httpRequest(float v1, float v2, float v3, float v4) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    ftoa(t1, v1, 2);
    ftoa(t2, v2, 2);
    ftoa(t3, v3, 2);
    ftoa(t4, v4, 2);
    sprintf (buffer, "GET /putData?t1=%s&t2=%s&t3=%s&t4=%s&key=YOURPASSWORD HTTP/1.1", t1, t2, t3, t4);
    client.println( buffer );
    client.println("Host: APPENGINEID");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    httpStatus = "Y";

    // note the time that the connection was made:
    nextTime = postTimesAttempts + 1;
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
    httpStatus = "N";
    //serverCode = "";
  }
}

void loop() {
    sensors.requestTemperatures();
    float t1 = sensors.getTempCByIndex(0);
    float t2 = sensors.getTempCByIndex(1);
    float t3 = sensors.getTempCByIndex(2);
    float t4 = sensors.getTempCByIndex(3);

    lcd.clear();
    lcd.setCursor(0,0);

    lcd.print(httpStatus);

    lcd.print(t1);
    lcd.print("C ");
    lcd.print(t2);
    lcd.print("C ");

    lcd.setCursor(0, 1);

    lcd.print(" ");

    lcd.print(t3);
    lcd.print("C ");

    lcd.print(t4);
    lcd.print("C ");

    // if there's incoming data from the net connection.
    if (client.available()) {
      int i = 0;
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    // if there's no net connection, but there was one last time
    // through the loop, then stop the client:
    if (!client.connected() && lastConnected) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
    }

    // if you're not connected, and ten seconds have passed since
    // your last connection, then connect again and send data:
    postTimesAttempts = millis() / postingInterval;
    if( !client.connected() && ( postTimesAttempts >= nextTime ) ) {
      Serial.println( millis() );
      httpRequest(t1, t2, t3, t4);
    }
    // store the state of the connection for next time through
    // the loop:
    lastConnected = client.connected();

}
