#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "tubesiot-ebb3d.firebaseio.com"
#define FIREBASE_AUTH "YEr4boJ4MFxZXhA9jUfJ6NHSIavxWM0A19AqjVDc"
#define WIFI_SSID "rexus"
#define WIFI_PASSWORD "Abi180798"

#define trigPinDalam D7
#define echoPinDalam D6
#define LED D2
int n = 0;

long distance,duration,jarakDalam;
//Servo servo;

WiFiServer server(80);
String header;
String ipwifi;

void setup() {
  Serial.begin(9600);
  //pinMode trig & echo dalam dan luar
  pinMode(trigPinDalam, OUTPUT);
  pinMode(echoPinDalam, INPUT);
  //pinMode LED
  pinMode(LED, OUTPUT);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(WIFI_SSID);
  Serial.println(WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("connected: ");
  ipwifi=WiFi.localIP().toString();
  Serial.println(ipwifi);
  
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.stream("/automation");
  server.begin();
}

void loop() {
  Firebase.setString("ip", ipwifi);
  WiFiClient client = server.available();
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
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

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>h1,h2,h3 { font-family: Helvetica;  margin: 20px auto; text-align: center;}");
            
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Smart Trash Can Control</h1>");
            client.println("<body><h2>Wemos Web Server</h3>");
            
            client.print("<h3>Status: " + Firebase.getString("capacity") + "</h3>");
            client.println("<body><br><br><br><br><br><br>");
            client.println("<body><h4>Nama Kelompok:</h4>");
            client.println("<body><p>Habiburrahman (F1D016034)</p>");
            client.println("<body><p>Kurnia Mulia K (F1D016048)</p>");
            client.println("<body><p>Nurhaini R (F1D016066)</p>");
            client.println("<body><p>Salma Nabilla U (F1D016080)</p>");
            client.println("<body><p>Baiq Alung(F1D016014) </p>");
               
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
  //sensor jarak sampah
  SonarSensor(trigPinDalam, echoPinDalam);
  jarakDalam = distance;

  if (jarakDalam >= 10 || jarakDalam <= 0){
    if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
    }else{
      digitalWrite(LED, LOW);
      Serial.print(jarakDalam);Serial.println(" cm");
      Firebase.setInt("distance", jarakDalam);
      Firebase.setString("capacity", "Belum Penuh");
      Serial.print("capacity: ");Serial.println(Firebase.getString("capacity"));
      delay(500);
    }
  }else{
    if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
    }else{
      digitalWrite(LED, HIGH);
      Serial.print(jarakDalam);Serial.println(" cm");
      Firebase.setInt("distance", jarakDalam);
      Firebase.setString("capacity", "Penuh");
      Serial.print("capacity: ");Serial.println(Firebase.getString("capacity"));
      delay(500);
    }
  }delay(500);
}

void SonarSensor(int trigPin,int echoPin)
{
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
distance = (duration/2) / 29.1;
}
