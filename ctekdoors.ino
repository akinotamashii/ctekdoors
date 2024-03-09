/*
C-Tek Door Listener
by: Aki

Designed for an Adafruit Feather MO Wifi - ATSAMD21 + ATWINC1500 
with attached NeoPixel FeatherWing - 4x8 RGB LED

This polls a URL every x milliseconds (defined by postingInterval) and checks if a door is open

TODO
Determine what door we are
make sure we are checking the status of the right door, not just a generic URL
 */


#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h" 
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

///////use arduino_secrets.h
char ssid[] = SECRET_SSID;        
char pass[] = SECRET_PASS;    
int keyIndex = 0;           

int status = WL_IDLE_STATUS;

bool doorstatus = false;

//NeoPixel setup
#define LED_PIN    6
#define LED_COUNT 32

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 4, 6,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);



//IPAddress server(74,125,232,128);  // numeric IP (if you want to avoid DNS)
char server[] = "futurfox.aki.fyi";
int port = 8888; 

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 2000L; // delay between updates, in milliseconds

WiFiClient client;

void setup() {
  //Configure pins for Adafruit Feather M0
  WiFi.setPins(8,7,4,2);
  matrix.begin();
  matrix.setBrightness(10);
  matrix.fill(0x00fcfc00, 0, 8);
  matrix.fill(0x0000fcfc, 8, 8);
  matrix.fill(0x00fc00fc, 16, 8);
  matrix.fill(0x00fc0000, 24, 8);
  matrix.show();
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

}


void printWiFiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void loop() {

  while (client.available()) {
    char c = client.read();
    if (c == '0') {
      Serial.write("Door is Locked \n");
      matrix.fill(0x00ff0000, 0, 32);
      matrix.show();
      doorstatus = true;
    }
    else if (c == '1') {
      Serial.write("Door is Open \n");
      matrix.fill(0x0000ff00, 0, 32);
      matrix.show();
      doorstatus = true;
    }
    else if (doorstatus == false){
      Serial.write("Unknown Status \n");
      matrix.fill(0x0003fcfc, 0, 32);
      matrix.show();
    }
    
  }

  if (millis() - lastConnectionTime > postingInterval) {
    getdoorstatus();
  }
}

void getdoorstatus() {
  // close any connection before send a new request.
  client.stop();

  // if there's a successful connection
  if (client.connect(server, port)) {
    doorstatus = false;
    client.println("GET /");
    client.println();

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if we couldn't make a connection
    Serial.println("connection failed");
    matrix.fill(0x00fc8803, 0, 32);
    matrix.show();
    delay(3000);
  }
}





