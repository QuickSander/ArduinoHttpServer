//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 23-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! HTTP Request based on Stream

#include "StreamHttpRequest.hpp"

#include "HttpField.hpp"
#include "Debug.h"

#include <Arduino.h>

//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief Constructor. sets Stream timeout for reading data.
ArduinoHttpServer::StreamHttpRequest::StreamHttpRequest(Stream& stream) :
    m_stream(stream),
    m_body({0}),
    m_method(METHOD_INVALID),
    m_resource(),
    m_contentTypeField(),
    m_contentLengthField(),
    m_version(),
    m_result(RESULT_OK)
{
    m_stream.setTimeout(LINE_READ_TIMEOUT_MS);
}

//------------------------------------------------------------------------------
//! \brief Wait for data to become available on Stream and parses the request.
bool ArduinoHttpServer::StreamHttpRequest::readRequest()
{

   char lineBuffer[MAX_LINE_SIZE] = {0};

   int attempts(0);
   while(!m_stream.available())
   {
      // Quit when failed to retrieve data after n retries.
      if(attempts >= MAX_RETRIES_WAIT_DATA_AVAILABLE)
      {
         setError("Time out while waiting for data to become available.");
         break;
      }

      delay(10);
      ++attempts;
   }

   // Read complete line.
   if(readLine(lineBuffer))
   {
      // Parse the request header (first line).
      parseRequest(lineBuffer);

      // Keep parsing fields until an empty line has been found (with only new line / carriage return).
      while (readLine(lineBuffer))
      {
         parseField(lineBuffer);
      }

      DEBUG_PRINTLN("HTTP field parsing complete.");

      // Parse body.
      int contentLength(getContentLength());
      if (contentLength > MAX_BODY_LENGTH)
      {
         DEBUG_PRINTLN("Content-Length larger then the maximum content we can consume. Trunkating body.");
         contentLength = MAX_BODY_LENGTH;
      }

      DEBUG_PRINT("Content-Length: ");
      DEBUG_PRINTLN(contentLength);

      if(contentLength > 0)
      {
         DEBUG_PRINT("Parsing body .... ");
         m_stream.readBytes(m_body, contentLength);
         DEBUG_PRINTLN("done");
      }
   }

   return m_result == RESULT_OK;
}


//------------------------------------------------------------------------------
//! \brief Read a single line from Stream into _linebuffer_
bool ArduinoHttpServer::StreamHttpRequest::readLine(char linebuffer[MAX_LINE_SIZE])
{
    if(m_result!=RESULT_OK) { return false; }

    int bytesRead(0);

    memset(linebuffer, 0, MAX_LINE_SIZE);
    bytesRead = m_stream.readBytesUntil('\r', linebuffer, MAX_LINE_SIZE);

    // Read away remaining new-line and carriage returns
    for (int i=0; i<2 && m_stream.read() != '\n'; i++)
    {
        // Allow SoftwareSerial to process incomming data.
        delay(2);
    }

    return bytesRead > 0 ;
}

//------------------------------------------------------------------------------
//! \brief Parse first line of HTTP request.
void ArduinoHttpServer::StreamHttpRequest::parseRequest(char lineBuffer[])
{
    parseMethod(lineBuffer);
    parseResource();
    parseVersion();
}

//------------------------------------------------------------------------------
//! \brief Parse method: GET, PUT, HEAD, etc.
void ArduinoHttpServer::StreamHttpRequest::parseMethod(char lineBuffer[])
{
    if(m_result!=RESULT_OK) { return; }

    // First strtok call, initialize with cached line buffer.
    String token(strtok(lineBuffer, " "));

   if(token == "GET")
   {
      m_method = METHOD_GET;
   }
   else if (token == "PUT")
   {
      m_method = METHOD_PUT;
   }
   else if (token == "POST")
   {
      m_method = METHOD_POST;
   }
   else if (token == "HEAD")
   {
      m_method = METHOD_HEAD;
   }
   else
   {
      m_method = METHOD_INVALID;
      setError(String("Cannot handle HTTP method: \"")+token+"\".");
   }
}

//! Parse "HTTP/1.1" (or any other version).
void ArduinoHttpServer::StreamHttpRequest::parseVersion()
{
    if(m_result!=RESULT_OK) { return; }

    String version(strtok(0, " "));
    int slashPosition = version.lastIndexOf('/');
    if (slashPosition < version.length() && slashPosition > 0)
    {
        m_version = version.substring(slashPosition);
    }
    else
    {
        String message("Parse error. Invalid HTTP version: \"");
        message += version;
        message += "\".";
        setError(message);
    }

}

void ArduinoHttpServer::StreamHttpRequest::parseResource()
{
   if(m_result!=RESULT_OK) { return; }

    String resource( strtok(0, " ") );
    m_resource = ArduinoHttpServer::HttpResource(resource);

    if (!m_resource.isValid())
    {
        setError("Parse error. No resource specified.");
    }
}

void ArduinoHttpServer::StreamHttpRequest::neglectToken()
{
    strtok(0, " ");
}


void ArduinoHttpServer::StreamHttpRequest::parseField(char lineBuffer[])
{
   if(m_result!=RESULT_OK) { return; }

   ArduinoHttpServer::HttpField httpField(lineBuffer);

   if(httpField.getType() == ArduinoHttpServer::HttpField::TYPE_CONTENT_TYPE)
   {
      m_contentTypeField = httpField;
   }
   else if(httpField.getType() == ArduinoHttpServer::HttpField::TYPE_CONTENT_LENGTH)
   {
      m_contentLengthField = httpField;
   }
   else
   {
      // Ignore other fields for now.
   }
}

const char * const ArduinoHttpServer::StreamHttpRequest::getBody()
{
   DEBUG_PRINT("Body: ");
   DEBUG_PRINTLN(m_body);
   return m_body;
}

const String& ArduinoHttpServer::StreamHttpRequest::getErrorDescrition()
{
   return m_errorDescription;
}

void ArduinoHttpServer::StreamHttpRequest::setError(const String& errorMessage)
{
   m_result = RESULT_ERROR;
   m_errorDescription = errorMessage;
}
