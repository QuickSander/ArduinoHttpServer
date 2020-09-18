//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! A single header field.

#ifndef __ArduinoHttpServer__HttpField__
#define __ArduinoHttpServer__HttpField__

#include "FixString.hpp"

#include "Arduino.h"

namespace ArduinoHttpServer
{

//! A single HTTP field.
class HttpField
{

public:

   using SubValueString = FixString<128>;

   enum class Type: char
   {
      NOT_SUPPORTED,
      CONTENT_TYPE,
      CONTENT_LENGTH,
      USER_AGENT,
      AUTHORIZATION
   };

   HttpField(const char* fieldLine);
   HttpField();
   virtual ~HttpField();

   HttpField& operator=(const HttpField& other) = default;
   HttpField& operator=(HttpField&& other) = default;
   HttpField(const HttpField& other) = delete;

   const Type getType() const;

   inline const String& getValueAsString() const {return m_value; };
   const SubValueString getSubValueString(size_t subValueIndex) const;
   inline const int getValueAsInt() const {return m_value.toInt(); };

private:
   void determineType(const String& typeStr);

   static const char *SEPERATOR;
   static const char* SUB_VALUE_SEPERATOR;
   static const char* CONTENT_TYPE_STR;
   static const char* CONTENT_LENGTH_TYPE_STR;
   static const char* USER_AGENT_TYPE_STR;
   static const char* AUTHORIZATION_TYPE_STR;

   Type m_type;
   String m_value;

};

}

#endif // defined(__ArduinoHttpServer__HttpField__)
