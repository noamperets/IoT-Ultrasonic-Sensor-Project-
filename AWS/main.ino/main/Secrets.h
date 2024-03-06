#include <pgmspace.h>
 
#define SECRET
// All private information & keys are censored. Please fill it accordingly.

 
const char WIFI_SSID[] = "********"; // WiFi NAME       
const char WIFI_PASSWORD[] = "*******"; // WiFi Password
 
#define THINGNAME "ESP8266"
 
//int8_t TIME_ZONE = -5
 
const char MQTT_HOST[] = "**********"; // AWS IoT core -> Settings -> copy endpoint here

 // Amazon Root CA 1
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
********
-----END CERTIFICATE-----
)EOF";
 
 
// Device Certificate
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
*******
-----END CERTIFICATE-----
)KEY";
 
 
// Device Private Key
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
*******
-----END RSA PRIVATE KEY-----
)KEY";