// Include necessary libraries
#include <ESP8266WiFi.h> // Include the ESP8266 WiFi library
#include <WiFiClientSecure.h> // Include the WiFiClientSecure library for secure communication
#include <PubSubClient.h> // Include the PubSubClient library for MQTT communication
#include <ArduinoJson.h> // Include the ArduinoJson library for working with JSON
#include <time.h> // Include the time library for working with time functions
#include "secrets.h" // Include a header file containing sensitive information (WiFi credentials, certificates, etc.)


// Define the time zone offset in hours
#define TIME_ZONE 2 // Define the time zone offset in hours

// Define pins for the ultrasonic distance sensor (HC-SR04)
#define TRIGGER_PIN 5  // GPIO 5 (D1) - Define the trigger pin for the ultrasonic sensor
#define ECHO_PIN 4     // GPIO 4 (D2) - Define the echo pin for the ultrasonic sensor

// Global variables
float distance;  // Variable to store the distance value
unsigned long lastMillis = 0; // Variable to store the last time a task was performed
const long interval = 2000; // Interval for performing tasks (2 seconds)

// Define AWS IoT topics
#define AWS_IOT_PUBLISH_TOPIC   "esp8266/pub" // Define the MQTT publish topic
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub" // Define the MQTT subscribe topic

WiFiClientSecure net; // Create a secure WiFi client

// Load CA certificate, client certificate, and private key from secrets.h
BearSSL::X509List cert(cacert); // Load CA certificate
BearSSL::X509List client_crt(client_cert); // Load client certificate
BearSSL::PrivateKey key(privkey); // Load private key

PubSubClient client(net); // Create an MQTT client

time_t now; // Variable to store the current time
time_t nowish = 1510592825;  // An arbitrary time to check against during NTP synchronization

// Function to connect to NTP server and set the time
void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "il.pool.ntp.org", "time.nist.gov"); // Configure time settings
  now = time(nullptr);
  while (now < nowish) // Wait until the time is set correctly
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

// Callback function for handling incoming MQTT messages
void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Function to connect to AWS IoT
void connectAWS()
{
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);// Connect to WiFi

  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));

  while (WiFi.status() != WL_CONNECTED)// Wait until connected to WiFi
  {
    Serial.print(".");
    delay(1000);
  }

  NTPConnect(); // Connect to NTP server and set the time

  // Set certificates and keys for secure communication
  net.setTrustAnchors(&cert); // Set CA certificate for secure communication
  net.setClientRSACert(&client_crt, &key); // Set client certificate and private key for secure communication

  // Configure MQTT client
  client.setServer(MQTT_HOST, 8883); // Configure MQTT server
  client.setCallback(messageReceived); // Set callback function for incoming messages

  Serial.println("Connecting to AWS IoT");

  while (!client.connect(THINGNAME)) // Connect to AWS IoT
  {
    Serial.print(".");
    delay(1000);
  }

  if (!client.connected()) {// Check if connection is successful
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

// Function to publish distance data to AWS IoT
void publishMessage()
{
  StaticJsonDocument<200> doc;

  // Get current time
  now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  // Format time as a string
  char formattedTime[20];
  strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", &timeinfo);

  // Build JSON message with time and distance
  doc["time"] = formattedTime; //millis();
  doc["distance"] = distance;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  // Publish message to AWS IoT
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup() // Setup function
{
  Serial.begin(115200); // Begin serial communication
  connectAWS(); // Connect to AWS IoT

  pinMode(TRIGGER_PIN, OUTPUT); // Initialize the trigger pin for the ultrasonic sensor
  pinMode(ECHO_PIN, INPUT); // Initialize the echo pin for the ultrasonic sensor
}

void loop()
{
  // Measure distance using the ultrasonic sensor
  digitalWrite(TRIGGER_PIN, LOW); // Set trigger pin low
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH); // Set trigger pin high
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW); // Set trigger pin low
  distance = pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;  // Convert pulse duration to distance in centimeters

  if (distance >= 2 && distance <= 400) // Ignore values outside the valid measurement range
  {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(1000);
    now = time(nullptr);
  }

  // Check if MQTT client is disconnected and reconnect if needed
  if (!client.connected())
  {
    connectAWS();// Reconnect to AWS IoT if not connected
  }
  else
  {
    // Maintain the MQTT connection
    client.loop();

    // Publish distance data periodically
    if (distance >= 2 && distance <= 401) // Ignore values outside the valid measurement range
    {
      if (millis() - lastMillis > 5000)
      {
        lastMillis = millis();
        publishMessage();
      }
    }
  }
}
