#include <ArduinoHttpServer.h>

#include <Arduino.h>


#ifdef ESP8266 // This example is compatible with both, ATMega and ESP8266
   #include <ESP8266WiFi.h>
#else
   #include <SPI.h> //! \todo Temporary see fix: https://github.com/platformio/platformio/issues/48
   #include <WiFi.h>
#endif

const char* ssid = "";
const char* password = "";

WiFiServer wifiServer(80);

void setup()
{
   Serial.begin(115200);
   Serial.println("Starting Wifi Connection...");

   WiFi.begin(const_cast<char*>(ssid), password);
   while (WiFi.status() != WL_CONNECTED)
   {
      delay(500);
   }

   wifiServer.begin();
}

void loop()
{
   WiFiClient client( wifiServer.available() );
   if (client.connected())
   {
      // Connected to client. Allocate and initialize StreamHttpRequest object.
      ArduinoHttpServer::StreamHttpRequest<1024> httpRequest(client);

      if (httpRequest.readRequest())
      {

         // Retrieve HTTP resource / URL requested
         Serial.println( httpRequest.getResource().toString() );

         // Retrieve 1st part of HTTP resource.
         // E.g.: "api" from "/api/sensors/on"
         Serial.println(httpRequest.getResource()[0]);
         
      }
      else
      {
         // HTTP parsing failed. Client did not provide correct HTTP data or
         // client requested an unsupported feature.
         ArduinoHttpServer::StreamHttpErrorReply httpReply(client, httpRequest.getContentType());

         const char *pErrorStr( httpRequest.getError().cStr() );
         String errorStr(pErrorStr); //! \todo Make HttpReply FixString compatible.

         httpReply.send( errorStr );
      }
      client.stop();
   }

}
