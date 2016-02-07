//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! Debug support

//#define ARDUINO_HTTP_SERVER_DEBUG

#ifdef ARDUINO_HTTP_SERVER_DEBUG
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINT(...) Serial.print(__VA_ARGS__)
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINT(...)
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(...)
#endif
