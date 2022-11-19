//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! Debug support
#ifdef ARDUINO_HTTP_SERVER_DEBUG
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINT(...) Serial.print(__VA_ARGS__)
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINT(...)
   #define DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(...)
#endif


#ifdef ARDUINO_HTTP_SERVER_NO_FLASH
   #define AHS_F(x) (x)
#else
   #define AHS_F(x) F(x)
#endif
