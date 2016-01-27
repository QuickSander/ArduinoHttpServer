//
//  HttpField.h
//  WifiESP8266SmartSwitch
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2015 Sander van Woensel. All rights reserved.
//

#ifndef __ArduinoHttpServer__HttpField__
#define __ArduinoHttpServer__HttpField__

#include "Arduino.h"

namespace ArduinoHttpServer
{

//! A single HTTP field.
class HttpField
{

public:

   enum TypeEnum
   {
      TYPE_NOT_SUPPORTED,
      TYPE_CONTENT_TYPE,
      TYPE_CONTENT_LENGTH,
      TYPE_USER_AGENT

   };

   HttpField(const char* fieldLine);
   HttpField();
   ~HttpField();

   HttpField& operator=(const HttpField& other);

   const TypeEnum getType() const;

   inline operator const String&() const { return m_value; }
   inline operator int() const { return m_value.toInt(); }

private:
   void determineType(const String& typeStr);

   static const char *SEPERATOR;

   TypeEnum m_type;
   String m_value;

};

}

#endif // defined(__ArduinoHttpServer__HttpField__)
