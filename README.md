# ArduinoHttpServer

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/d6c43caaa5e24ebd9cf2119ac7963ce6)](https://app.codacy.com/app/QuickSander/ArduinoHttpServer?utm_source=github.com&utm_medium=referral&utm_content=QuickSander/ArduinoHttpServer&utm_campaign=Badge_Grade_Dashboard)
[![Build Status](https://travis-ci.org/QuickSander/ArduinoHttpServer.svg?branch=master)](https://travis-ci.org/QuickSander/ArduinoHttpServer)
[![Code Climate](https://codeclimate.com/github/QuickSander/ArduinoHttpServer/badges/gpa.svg)](https://codeclimate.com/github/QuickSander/ArduinoHttpServer)
[![Test Coverage](https://codeclimate.com/github/QuickSander/ArduinoHttpServer/badges/coverage.svg)](https://codeclimate.com/github/QuickSander/ArduinoHttpServer/coverage)




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
   // Retrieve 4th part (index is zero based) of the resource URL.
   // E.g. state from: "/api/sensors/1/state"
   const String& restFunction( httpRequest.getResource()[3] );
}
```


### Writing an HTTP reply to some Stream
```c++
ArduinoHttpServer::StreamHttpReply httpReply(Serial, "application/json");
httpReply.send("{\"All your base are belong to us!\"}");
```

Documentation
-------------

Documentation available in the [ArduinoHttpServer Github wiki](https://github.com/QuickSander/ArduinoHttpServer/wiki)

Characteristics
---------------
* HTTP parser with protocol validation.
* Puts you in control on how to react on a HTTP request; no implicit behaviour.
* Customizable memory footprint for caching returned body data.
* No external dependencies outside of the standard Arduino framework.
* Object oriented implementation.
