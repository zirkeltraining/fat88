// fat88
// a 2 x 7-segment-display from smartLEDs 
// 
// Modes:
// A - Strand Test. Simply scrolls through all of the leds.
// B #ffffff/2,#000000/3 - will fill the string with 2 white and 3 black leds, alternatingly.
// C[nn] with n = 0...9,A,b,C,d,E,F,g,h,i,J,K,L,-,n,o,P,q,r,S,t,U,v,-,-,y,Z - show character 
// F[int] = set the fade time in milliseconds
// T = show clock. Shows the real time.
// T[hhmm] = set the clock.
// S = Stopwatch. Starts with seconds, then progresses to minutes
// D[secs] = CountDown. While secs>60, shows the minutes. 
// R[hhmm] = set the alaRm.
// 0=do nothig





// Values
// numLEDS = 42
// functions:
// interrrupt: trying to call this with about 50Hz.
//      fades the content of curValues based on fadeSpeed and updateTimestamp towards the futValues

#define DEBUG 1
//#define NODEMCU

#define wsPin 10 
// on an arduino we use pin 10. // on a nodemcu, use D0 (ESP8266 pin GPIO16)

#ifdef NODEMCU 
 #include <ESP8266WiFi.h>
 WiFiServer server(80);
#endif


#include <Adafruit_NeoPixel.h>
const char* ssid = "r199 Guest Network";
const char* password = "seiunsergast";



 
#define numLEDs 42
int btnPin = 11; // this would be a button to toggle the various modes
int i=0;
unsigned long timer=0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char mode ='C';                 // stores the mode of the entire device. 
uint32_t vt[numLEDs];     // target value
unsigned long countDown;
float delta;
char buf[12];
char a;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLEDs, wsPin, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
    // reserve 200 bytes for the inputString:
  inputString.reserve(500);
  delay(10);
 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH); //!digitalRead(LED_BUILTIN)

  pinMode(wsPin, OUTPUT);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("Resetting LEDs");

  #ifdef NODEMCU
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
   
    WiFi.begin(ssid, password);
   
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
   
    // Start the server
    server.begin();
    Serial.println("Server started");
   
    // Print the IP address
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
  #endif
  
  // DEBUG CODE
  #ifdef DEBUG
    Serial.println("lets show 42");
    set7Seg('4','2');
  #endif
  
  mode='0';
}
 
void loop() {

  // latzily turn the control LED on ledPin (onboard) on 'n' off
  if (millis()>(timer+2000)) {
     timer=millis();
     //digitalWrite(LED_BUILTIN,HIGH); //!digitalRead(LED_BUILTIN)
     #ifdef DEBUG
      Serial.println(millis());
     #endif
    }
    
  // parse the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    if ((inputString.length()>2) && (inputString.charAt(0)=='C')){ // char mode
        mode='C';   
        set7Seg(inputString.charAt(1),inputString.charAt(2)); // 
      } else
      if (inputString.charAt(0)=='D') {
        mode='D';
        countDown=millis()+99000;
        Serial.println("Doing a countdown");
      }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  // do something according to the mode.
  // currently only C and D are working
  
  switch (mode) {
    case 'D':
      delta=countDown-millis();
      if (delta<1000) {
        mode='0';
        delta=0;
        Serial.println("Countdown done.");
        }  
      itoa(int(delta/10000),buf,10);
      a=buf[0];
      itoa((int(delta/1000)%10),buf,10);
      set7Seg(a,buf[0]);
    break;
    
    case 'C':
      // nothing to do here.
    break;

// now work on the color.


  
}
  
  #ifdef NODEMCU
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    for (i=0 ; i<numLEDs ; i++) {
      strip.setPixelColor(i, strip.Color(255,255,255));
    }
    strip.show();
    mode='0';
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    for (i=0 ; i<numLEDs ; i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
    }
    strip.show();
    mode='0';
    value = LOW;
  }
  
  if (request.indexOf("/LED=TEST") != -1) {
    mode='C';
  }
  
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>All On</button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>All Off</button></a>");  
  client.println("<a href=\"/LED=TEST\"\"><button>Strand Test</button></a><br />");  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
  #endif
}
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

 
