#include <ArduinoHttpServer.h>

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
      ArduinoHttpServer::StreamHttpRequest<1023> httpRequest(client);

      // Parse the request.
      if (httpRequest.readRequest())
      {
         // Use the information you like they way you like.

         // Retrieve HTTP resource / URL requested
         Serial.println( httpRequest.getResource().toString() );

         // Retrieve 2nd part of HTTP resource.
         // E.g.: "on" from "/api/sensors/on"
         Serial.println( httpRequest.getResource()[2] );

         // Retrieve HTTP method.
         // E.g.: GET / PUT / HEAD / DELETE / POST
         ArduinoHttpServer::MethodEnum method( ArduinoHttpServer::MethodInvalid );
         method = httpRequest.getMethod();

         if( method == ArduinoHttpServer::MethodGet )
         {
            Serial.println("Nothing to get here.");
         }
         else if( method == ArduinoHttpServer::MethodPut )
         {
            digitalWrite(13, HIGH);
         }

      }
      else
      {
         // HTTP parsing failed. Client did not provide correct HTTP data or
         // client requested an unsupported feature.
         ArduinoHttpServer::StreamHttpErrorReply httpReply(client, httpRequest.getContentType());
         httpReply.send(httpRequest.getErrorDescrition());
      }
   }

}
