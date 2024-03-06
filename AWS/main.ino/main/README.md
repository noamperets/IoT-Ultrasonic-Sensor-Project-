# ESP8266 AWS IoT Distance Sensor

This Arduino sketch is designed for the ESP8266 microcontroller and implements a distance sensor using an ultrasonic sensor (HC-SR04). The measured distance is periodically published to AWS IoT using the MQTT protocol for remote monitoring.

## Prerequisites

Before uploading the code to your ESP8266, ensure you have the following:

- [Arduino IDE](https://www.arduino.cc/en/Main/Software) installed
- ESP8266 board support added to Arduino IDE
- Necessary libraries installed: ESP8266WiFi, WiFiClientSecure, PubSubClient, ArduinoJson

## Getting Started

1. Clone or download the repository to your local machine.

2. Open the `main.ino` file in the Arduino IDE.

3. Modify the `secrets.h` file to include your WiFi credentials, AWS IoT endpoint, and certificates.

```cpp
// secrets.h
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define MQTT_HOST "your_aws_iot_endpoint"
#define THINGNAME "your_thing_name"
const char cacert[] = "your_ca_certificate";
const char client_cert[] = "your_client_certificate";
const char privkey[] = "your_private_key";
```

4. Save the changes to `secrets.h`.

5. Upload the code to your ESP8266.

## Configuration

Adjust the following settings in `main.ino` according to your requirements:

- **TIME_ZONE:** Set the time zone offset in hours.
- **TRIGGER_PIN and ECHO_PIN:** Define the GPIO pins for the ultrasonic sensor.

## Usage

1. Power up your ESP8266.

2. The device will connect to the specified WiFi network, synchronize its time via NTP, and connect to AWS IoT.

3. The ultrasonic sensor will measure distance, and if within the valid range, the data will be published to the specified MQTT topic.

4. Monitor the AWS IoT console for incoming distance data.

## Notes

- Ensure the AWS IoT endpoint, certificates, and Thing Name are correctly configured in `secrets.h`.
- Adjust the measurement range and publish interval as needed in the `loop()` function.
- Serial monitor output provides information about WiFi connection, AWS IoT connection, and distance measurements.

Feel free to explore and customize the code for your specific use case. For any issues or questions, refer to the [ESP8266 documentation](https://arduino-esp8266.readthedocs.io/en/latest/) or [AWS IoT documentation](https://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html).
