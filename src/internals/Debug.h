//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! Debug support

#define ARDUINO_HTTP_SERVER_DEBUG

#ifdef ARDUINO_HTTP_SERVER_DEBUG
   #define DEBUG_BEGIN(baudrate) Serial.begin(baudrate)
   #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
   #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
   #define DEBUG_BEGIN
   #define DEBUG_PRINT(...)
   #define DEBUG_PRINTLN(...)
#endif
