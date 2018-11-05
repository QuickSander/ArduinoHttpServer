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
   m_type(Type::NOT_SUPPORTED),
   m_value()
{

   DEBUG_ARDUINO_HTTP_SERVER_PRINT("Parsing HTTP field: ");
   DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(fieldLine);

   //! \todo FixString
   String fieldStr(fieldLine);
   int fieldSepIndex( fieldStr.indexOf(SEPERATOR) );

   if( fieldSepIndex < 1 )
   {
      m_type = Type::NOT_SUPPORTED;
   }
   else
   {
      m_value = fieldStr.substring(fieldSepIndex + strlen(SEPERATOR));
      String typeStr = fieldStr.substring(0, fieldSepIndex);
      determineType(typeStr);

   }
}

ArduinoHttpServer::HttpField::HttpField() :
   m_type(Type::NOT_SUPPORTED),
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
      m_type = Type::CONTENT_TYPE;
   }
   else if (typeStr.equalsIgnoreCase(CONTENT_LENGTH_TYPE_STR))
   {
      m_type = Type::CONTENT_LENGTH;
   }
   else if (typeStr.equalsIgnoreCase(USER_AGENT_TYPE_STR))
   {
      m_type = Type::USER_AGENT;
   }
}


const ArduinoHttpServer::HttpField::Type ArduinoHttpServer::HttpField::getType() const
{
   return m_type;
}
