//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 20-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! A single header field.

#include "HttpField.hpp"
#include "ArduinoHttpServerDebug.h"

const char* ArduinoHttpServer::HttpField::SEPERATOR = ": ";
const char* ArduinoHttpServer::HttpField::CONTENT_TYPE_STR = "Content-Type";
const char* ArduinoHttpServer::HttpField::CONTENT_LENGTH_TYPE_STR = "Content-Length";
const char* ArduinoHttpServer::HttpField::USER_AGENT_TYPE_STR = "User-Agent";


ArduinoHttpServer::HttpField::HttpField(const char* fieldLine) :
   m_type(TYPE_NOT_SUPPORTED),
   m_value()
{

   DEBUG_ARDUINO_HTTP_SERVER_PRINT("Parsing HTTP field: ");
   DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(fieldLine);

   String fieldStr(fieldLine);
   int fieldSepIndex( fieldStr.indexOf(SEPERATOR) );

   if( fieldSepIndex < 1 )
   {
      m_type = TYPE_NOT_SUPPORTED;
   }
   else
   {
      m_value = fieldStr.substring(fieldSepIndex + strlen(SEPERATOR));
      String typeStr = fieldStr.substring(0, fieldSepIndex);
      determineType(typeStr);

   }
}

ArduinoHttpServer::HttpField::HttpField() :
   m_type(TYPE_NOT_SUPPORTED),
   m_value()
{
}

ArduinoHttpServer::HttpField::~HttpField()
{

}

void ArduinoHttpServer::HttpField::determineType(const String& typeStr)
{
   if (typeStr.equalsIgnoreCase(CONTENT_TYPE_STR))
   {
      m_type = TYPE_CONTENT_TYPE;
   }
   else if (typeStr.equalsIgnoreCase(CONTENT_LENGTH_TYPE_STR))
   {
      m_type = TYPE_CONTENT_LENGTH;
   }
   else if (typeStr.equalsIgnoreCase(USER_AGENT_TYPE_STR))
   {
      m_type = TYPE_USER_AGENT;
   }
}

ArduinoHttpServer::HttpField& ArduinoHttpServer::HttpField::operator=(const ArduinoHttpServer::HttpField& other)
{
   m_type = other.m_type;
   m_value = other.m_value;

   return *this;
}

const ArduinoHttpServer::HttpField::TypeEnum ArduinoHttpServer::HttpField::getType() const
{
   return m_type;
}
