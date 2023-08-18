
#include <ArduinoHttpServer.h>

#include <Arduino.h>


#ifdef ESP8266 // This example is compatible with both, ATMega and ESP8266
   #include <ESP8266WiFi.h>
#else
   #include <SPI.h> //! \todo Temporary see fix: https://github.com/platformio/platformio/issues/48
// on older Arduinos use WiFi.h, on Arduino Uno R4 WiFi use WiFiS3.h
   #include <WiFiS3.h>
#endif

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASS";

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
   Serial.println("Connected!");

   wifiServer.begin();
}

void loop()
{
   WiFiClient client( wifiServer.available() );
   if (client.connected())
   {
   Serial.println("client connected!");
      // Connected to client. Allocate and initialize StreamHttpRequest object.
      ArduinoHttpServer::StreamHttpRequest<1024> httpRequest(client);

      if (httpRequest.readRequest())
      {
        Serial.println("Done read request!");

         // Retrieve HTTP resource / URL requested
         Serial.println( httpRequest.getResource().toString() );

         // Retrieve 1st part of HTTP resource.
         // E.g.: "api" from "/api/sensors/on"
         Serial.println(httpRequest.getResource()[0]);
                   
          ArduinoHttpServer::StreamHttpReply httpReply(client, "text/html");
          String textField = httpRequest.getResource().getArgument("textfield");
         Serial.print("Textfield is: ");
         Serial.println(textField);
          
          String reply = "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "  <title>Minimal HTML Page</title>\n"
            "</head>\n"
            "<body>\n";
            if(textField.length() > 0) {
                reply += "<h3> Hello, ";
                reply += textField;
                reply += "!</h3>";
            }
            reply +=  "  <form>\n"
            "    <label for=\"textfield\">Enter text:</label><br>\n"
            "    <input type=\"text\" id=\"textfield\" name=\"textfield\"><br><br>\n"
            "    <input type=\"submit\" value=\"Submit\">\n"
            "  </form>\n"
            "</body>\n"
            "</html>";
         httpReply.send(reply);
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