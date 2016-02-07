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

ArduinoHttpServer::HttpResource& ArduinoHttpServer::HttpResource::operator=(const ArduinoHttpServer::HttpResource& other)
{
   m_resource = other.m_resource;

   return *this;
}


bool ArduinoHttpServer::HttpResource::isValid()
{
   return m_resource.length() > 0;
}

//! Retrieve resource part at the specified index.
//! \details E.g. HttpResource("/api/sensors/1/state")[1]
//!    returns "sensors"
String ArduinoHttpServer::HttpResource::operator[](const unsigned int index) const
{
   int fromOffset(0);

   // Forward till we reach desired index.
   for (unsigned int currentIndex=0; currentIndex <= index; ++currentIndex)
   {
      fromOffset = m_resource.indexOf(RESOURCE_SEPERATOR, fromOffset);
      ++fromOffset; // Seek past '/'.
   }

   // Find next possible '/' or end.
   int toOffset( m_resource.indexOf(RESOURCE_SEPERATOR, fromOffset) );
   toOffset == -1 ? m_resource.length() - 1 : toOffset;

   return m_resource.substring(fromOffset, toOffset);

}


const String& ArduinoHttpServer::HttpResource::toString() const
{
   return m_resource;
}
