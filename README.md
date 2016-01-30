# ArduinoHttpServer

[![Build Status](https://travis-ci.org/QuickSander/ArduinoHttpServer.svg?branch=master)](https://travis-ci.org/QuickSander/ArduinoHttpServer)

*Server side minimalistic Object Oriented HTTP protocol implementation for the Arduino platform.*

ArduinoHttpServer is a simple HTTP request and reply implementation targeted for the embedded Arduino platform. The implementation parses an HTTP request/reply reading/printing from/to any Stream (either Serial or Wifi/Ethernet).

Quick start
-----------

### Reading an HTTP request from some Stream
```c++
// This example uses the Stream instance Serial, might also be a WifiClient.
ArduinoHttpServer::StreamHttpRequest httpRequest(Serial);
bool success(httpRequest.readRequest())
if (success) // If no HTTP parsing error or read timeout occurred.
{
   const char *body( httpRequest.getBody() );
}


```


### Writing an HTTP reply to some Stream
