//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! An HTTP version

#ifndef __ArduinoHttpServer__HttpVersion__
#define __ArduinoHttpServer__HttpVersion__

#include "Arduino.h"

namespace ArduinoHttpServer
{

//! A single HTTP field.
class HttpVersion
{

public:
   HttpVersion(const String& version);
   HttpVersion();

   HttpVersion& operator=(const HttpVersion& rhs);

   String toString();
   inline unsigned char getMajor() const { return m_major; }
   inline unsigned char getMinor() const { return m_minor; }

private:
   static const char SEPARATOR;

   unsigned char m_major;
   unsigned char m_minor;

};

}

#endif
