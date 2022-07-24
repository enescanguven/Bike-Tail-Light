#include <ESP8266WiFi.h>

const char* ssid     = "$SSID";
const char* password = "$PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

String mode1State = "off";
String mode2State = "off";
String mode3State = "off";
int state= 0;
int i,a;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
unsigned long pTime = 0,p1Time=0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//neopixel

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        12 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();


  //neopixel

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();
  pixels.show();
}


void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /1/on") >= 0) {
              Serial.println("mode 1 on");
              mode1State = "on";
              
             
              
            } else if (header.indexOf("GET /1/off") >= 0) {
              Serial.println("mode 1 off");
              mode1State = "off";
              pixels.clear();
              pixels.show();
              
            } 
            
            else if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("mode 2 on");
              mode2State = "on";

            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("mode 2 off");
              mode2State = "off";
              pixels.clear();
              pixels.show();
            }
            
            if (header.indexOf("GET /3/on") >= 0) {
              Serial.println("mode 3 on");
              mode3State = "on";
              
             
              
            } else if (header.indexOf("GET /3/off") >= 0) {
              Serial.println("mode 3 off");
              mode3State = "off";
              pixels.clear();
              pixels.show();
              
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button3 {background-color: #77878A;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            
            // Web Page Heading
            client.println("<body><h1>Enes's Bike</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 1  
            client.println("<p>MODE 1 - State " + mode1State + "</p>");
            // If the output1State is off, it displays the ON button       
            if (mode1State=="off") {
              client.println("<p><a href=\"/1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/1/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 2  
            client.println("<p>MODE 2 - State " + mode2State + "</p>");
            // If the output2State is off, it displays the ON button       
            if (mode2State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 3  
            client.println("<p>MODE 3 - State " + mode3State + "</p>");
            // If the output2State is off, it displays the ON button       
            if (mode3State=="off") {
              client.println("<p><a href=\"/3/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/3/off\"><button class=\"button button3\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
              } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        
      }
    
      
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    

    
  }

if(mode1State == "on"){
   pixels.setPixelColor(0, pixels.Color(255, 0, 0));
   pixels.setPixelColor(1, pixels.Color(255, 0, 0));
   pixels.setPixelColor(2, pixels.Color(255, 0, 0));
   pixels.setPixelColor(3, pixels.Color(255, 0, 0));
   pixels.setPixelColor(4, pixels.Color(255, 0, 0));
   pixels.setPixelColor(5, pixels.Color(255, 0, 0));
   pixels.setPixelColor(6, pixels.Color(255, 0, 0));
   pixels.setPixelColor(7, pixels.Color(255, 0, 0));
   pixels.setPixelColor(8, pixels.Color(255, 0, 0));
   pixels.show();
  }


  else if(mode2State == "on" && millis() - pTime > 50){
    
    i++;
    a=i%9;
    
    pixels.setPixelColor(a, pixels.Color(255, 0, 0));
    pixels.show();
    pTime = millis();
    if(a==0){
      pixels.clear();
      pixels.show();
      i=0;
      }
    
    }
  else if(mode3State == "on" && millis() - p1Time > 200){
    p1Time = millis();
    
   if(state%2==0){
    state = 0;
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.setPixelColor(1, pixels.Color(255, 0, 0));
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    pixels.setPixelColor(4, pixels.Color(255, 0, 0));
    pixels.setPixelColor(5, pixels.Color(255, 0, 0));
    pixels.setPixelColor(6, pixels.Color(255, 0, 0));
    pixels.setPixelColor(7, pixels.Color(255, 0, 0));
    pixels.setPixelColor(8, pixels.Color(255, 0, 0));
    pixels.show();
    
    
    }
    if(state%2==1){
      
      pixels.clear();
      pixels.show();
      
      }
   state++;
  
  }
}
