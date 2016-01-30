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
// Reserve 511 bytes for body content storage.
ArduinoHttpServer::StreamHttpRequest<511> httpRequest(Serial);
bool success(httpRequest.readRequest())
if (success) // If no HTTP parsing error or read timeout occurred.
{
   // See interface api for other methods.
   const char *body( httpRequest.getBody() );
   // Retrieve 2nd part of the resource URL. E.g. state from: "/api/sensors/1/state"
   const String& restFunction( httpRequest.getResource()[1] );
}
```


### Writing an HTTP reply to some Stream
```c++
ArduinoHttpServer::StreamHttpReply<127> httpReply(Serial, "application/json");
httpReply.send("{\"All your base are belong to us!\"}");
```

Characteristics
---------------
* HTTP parser with with protocol validation.
* Puts you in control on how to react on a HTTP request; no implicit behaviour.
* Customizable memory footprint for caching returned body data.
* No external dependencies outside of the standard Arduino framework.
* Object oriented implementation.
