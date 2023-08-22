//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 23-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! The URL/path provided after the HTTP method.

#include "HttpResource.hpp"

#include <WString.h>

ArduinoHttpServer::HttpResource::HttpResource(const String& resource) :
   m_resource(resource)
{
}

ArduinoHttpServer::HttpResource::HttpResource() :
   m_resource()
{
}

// TODO: default?
ArduinoHttpServer::HttpResource& ArduinoHttpServer::HttpResource::operator=(const ArduinoHttpServer::HttpResource& other)
{
   m_resource = other.m_resource;

   return *this;
}


bool ArduinoHttpServer::HttpResource::isValid()
{
   return m_resource.length() > 0;
}
//! Retrieve the GET argument value for given string key
//! \details E.g. HttpResource("/index.php&key=value").getArgument("key")
//!    returns "value".
//! \returns Empty string when index specified is out of range. 
String ArduinoHttpServer::HttpResource::getArgument(const char *key) const {
   int queryStart = m_resource.indexOf('?');
   if (queryStart == -1) {
      return "";
   }
   
   queryStart++;

	// FIXME: adding = will ensure that = is directly after key name,
	// but we still may get incorrect values for strings like "akey" vs "key"...
   String keyStr = String(key) + '=';
   int keyStart = m_resource.indexOf(keyStr, queryStart);

   if (keyStart == -1) {
      return "";
   }

   int valueEnd = m_resource.indexOf('&', keyStart);
   if (valueEnd == -1) {
      valueEnd = m_resource.length();
   }
#if 0
   String ret = m_resource.substring(keyStart + keyStr.length(), valueEnd);
#else
	int a, b;
	String ret = "";
	for(int i = keyStart + keyStr.length(); i < valueEnd; i++) {
		if (i < valueEnd-3 && (m_resource[i] == '%') &&
			((a = m_resource[i+1]) && (b = m_resource[i+2])) &&
			(isxdigit(a) && isxdigit(b))) {
			if (a >= 'a')
				a -= 'a' - 'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a' - 'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			ret += (char)(16 * a + b);
			i += 2;
		}
		else if (m_resource[i] == '+') {
			ret += ' ';
		}
		else {
			ret += m_resource[i];
		}
	}
#endif
   return ret;
}
//! Retrieve resource part at the specified index.
//! \details E.g. HttpResource("/api/sensors/1/state")[1]
//!    returns "sensors".
//! \returns Empty string when index specified is out of range. 
String ArduinoHttpServer::HttpResource::operator[](const unsigned int index) const
{
   int fromOffset(0);

   // Forward till we reach desired index.
   for (unsigned int currentIndex=0; currentIndex <= index; ++currentIndex)
   {
      fromOffset = m_resource.indexOf(RESOURCE_SEPERATOR, fromOffset);
      if(fromOffset == -1)
      {
         return String("");
      }

      ++fromOffset; // Seek past '/'.
   }

   // Find next possible '/' or end.
   int toOffset( m_resource.indexOf(RESOURCE_SEPERATOR, fromOffset) );
   if(toOffset == -1)
   {
      toOffset = m_resource.length();
   }

   return m_resource.substring(fromOffset, toOffset);

}


const String& ArduinoHttpServer::HttpResource::toString() const
{
   return m_resource;
}
