# ArduinoHttpServer

[![GitHub Actions](https://github.com/QuickSander/ArduinoHttpServer/actions/workflows/main.yml/badge.svg?event=push)](https://github.com/QuickSander/ArduinoHttpServer/actions/workflows/main.yml)
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
// Reserve 512 bytes for body content storage.
ArduinoHttpServer::StreamHttpRequest<512> httpRequest(Serial);
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
Article by P. Kaczmarek: [Arduino R4 WiFi and ArduinoHttpServer - fixes, launch, examples of use](https://www.elektroda.com/rtvforum/topic3996584.html)

### Compiler options
The library can be adjusted to include less or more features depending on the board you use and your desires for hardware usage. Specify these in your project's ```platformio.ini``` via the [```build_flags```](https://docs.platformio.org/en/latest/projectconf/section_env_build.html#build-flags) argument or as ```#define``` in your source/header code before including ```ArduinoHttpServer.h```.

| ```#define``` | Description |
| ------------- | ----------- |
| ```ARDUINO_HTTP_SERVER_DEBUG``` | Enable debug logging printed towards the default Serial port |
| ```ARDUINO_HTTP_SERVER_NO_FLASH``` | Do not put string literals used inside the library's implementation in flash memory. Increases RAM usage, decreases flash usage. |
| ```ARDUINO_HTTP_SERVER_NO_BASIC_AUTH``` | Disable HTTP basic authentication support. Removes the need for the Base64 library. |

Characteristics
---------------
* HTTP parser with protocol validation.
* Puts you in control on how to react on a HTTP request; no implicit behaviour.
* Customizable memory footprint for caching returned body data.
* No external dependencies outside of the standard Arduino framework.
* Object oriented implementation.
* Basic authorization support (initial version by [Tomer-W](https://github.com/tomer-w))

