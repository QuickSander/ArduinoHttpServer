//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! HTTP version

#include "HttpVersion.hpp"
#include "ArduinoHttpServerDebug.h"

const char ArduinoHttpServer::HttpVersion::SEPARATOR = '.';

ArduinoHttpServer::HttpVersion::HttpVersion() :
   m_major(0),
   m_minor(0)
{
}

ArduinoHttpServer::HttpVersion::HttpVersion(const String& version) :
   m_major(0),
   m_minor(0)
{
   int dotIndex(version.lastIndexOf(SEPARATOR));

   // Cast might possibly invalidate version data when versions become bigger then 255.
   m_major = static_cast<unsigned char>( version.substring(0, dotIndex).toInt() );
   m_minor = static_cast<unsigned char>( version.substring(dotIndex+1, version.length()).toInt() );
}

ArduinoHttpServer::HttpVersion& ArduinoHttpServer::HttpVersion::operator=(const HttpVersion& rhs)
{
   m_major = rhs.m_major;
   m_minor = rhs.m_minor;

   return *this;
}

String ArduinoHttpServer::HttpVersion::toString()
{
   String version(m_major);
   version += SEPARATOR;
   version += m_minor;

   return version;
}
