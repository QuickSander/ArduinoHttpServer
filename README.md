# ArduinoHttpServer

[![Build Status](https://travis-ci.org/QuickSander/ArduinoHttpServer.svg?branch=master)](https://travis-ci.org/QuickSander/ArduinoHttpServer)

*Server side minimalistic Object Oriented HTTP protocol implementation for the Arduino platform.*

ArduinoHttpServer is a simple HTTP request and reply implementation targeted for the embedded Arduino framework. The implementation parses an HTTP request/reply reading/printing from/to any Stream (either Serial or Wifi/Ethernet).

What you do with the request or what you reply is entirely up to your imagination. Very little implicit behaviour.

Quick start
-----------

### Reading an HTTP request from some Stream instance
```c++
// This example uses the Stream instance Serial, might also be a WifiClient object.
ArduinoHttpServer::StreamHttpRequest httpRequest(Serial);
bool success(httpRequest.readRequest())
if (success) // If no HTTP parsing error or read timeout occurred.
{
   const char \*body( httpRequest.getBody() );
}
```


### Writing an HTTP reply to some Stream
```c++
ArduinoHttpServer::StreamHttpReply httpReply(Serial, "application/json");
httpReply.send("{\"All your base are belong to us!\"}");
```

Properties
----------
* HTTP parser with with protocol validation.
* No external dependencies outside of the standard Arduino framework.
* Object oriented implementation
