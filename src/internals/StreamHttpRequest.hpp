//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 24-02-15.
//  Copyright (c) 2015 Sander van Woensel. All rights reserved.
//
//! HTTP Request read from a Stream.

#ifndef __ArduinoHttpServer__StreamHttpRequest__
#define __ArduinoHttpServer__StreamHttpRequest__

#include "HttpResource.hpp"
#include "HttpField.hpp"
#include "HttpVersion.hpp"
#include "ArduinoHttpServerDebug.h"

#include <Arduino.h>

#include <string.h>

namespace ArduinoHttpServer
{

enum MethodEnum
{
   MethodInvalid, MethodGet, MethodPut, MethodPost, MethodHead
};


//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! HTTP request read from a _Stream_.
template <size_t MAX_BODY_LENGTH>
class StreamHttpRequest
{

public:
    StreamHttpRequest(Stream& stream);

    ~StreamHttpRequest() { };

    bool readRequest();

    // Header retrieval methods.
    inline const ArduinoHttpServer::HttpResource& getResource() const { return m_resource; };
    inline const MethodEnum getMethod() const { return m_method; };
    inline const ArduinoHttpServer::HttpVersion& getVersion() const { return m_version; };

    // Field retrieval methods.
    inline const String& getContentType() const { return m_contentTypeField.getValueAsString(); };
    inline const int getContentLength() const { return m_contentLengthField.getValueAsInt(); };

    // Body retrieval methods.
    inline const char* const getBody() const { return m_body; };

    // State retrieval
    const String& getErrorDescrition() { return m_errorDescription; };
    Stream& getStream() { return m_stream; };

private:

   enum ResultEnum {ResultError, ResultOk};

   //! \todo To reduce program memory size reduce these to the proper types: char and int.
   //!    Or better yet, make these Template variables.
   static const int MAX_LINE_SIZE = 255+1;
   static const int MAX_BODY_SIZE = MAX_BODY_LENGTH+1; //!< Byte size of array. Leaves space for terminating \0.
   static const long LINE_READ_TIMEOUT_MS = 10000L; //!< [ms] Wait 10s for reception of a complete line.
   static const int MAX_RETRIES_WAIT_DATA_AVAILABLE = 255;

   void parseRequest(char lineBuffer[MAX_LINE_SIZE]);
   void parseMethod(char lineBuffer[MAX_LINE_SIZE]);
   void parseResource();
   void parseVersion();
   void parseField(char lineBuffer[MAX_LINE_SIZE]);

   void neglectToken();

   bool readLine(char lineBuffer[MAX_LINE_SIZE]);

   void setError(const String& errorMessage);

   Stream& m_stream;
   char m_body[MAX_BODY_SIZE];
   MethodEnum m_method;
   ArduinoHttpServer::HttpResource m_resource;
   ArduinoHttpServer::HttpVersion m_version;
   ArduinoHttpServer::HttpField m_contentTypeField;
   ArduinoHttpServer::HttpField m_contentLengthField;

   ResultEnum m_result;
   String m_errorDescription;

   char *m_lineBufferStrTokContext;
};

}

//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief Constructor. sets Stream timeout for reading data.
template <size_t MAX_BODY_LENGTH>
ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::StreamHttpRequest(Stream& stream) :
    m_stream(stream),
    m_body{0},
    m_method(MethodInvalid),
    m_resource(),
    m_version(),
    m_contentTypeField(),
    m_contentLengthField(),
    m_result(ResultOk),
    m_errorDescription(),
    m_lineBufferStrTokContext(0)
{
   static_assert(MAX_BODY_LENGTH >= 0, "HTTP body length less then zero specified.");
   m_stream.setTimeout(LINE_READ_TIMEOUT_MS);
}

//------------------------------------------------------------------------------
//! \brief Wait for data to become available on Stream and parses the request.
template <size_t MAX_BODY_LENGTH>
bool ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::readRequest()
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

      DEBUG_ARDUINO_HTTP_SERVER_PRINTLN("HTTP field parsing complete.");

      // Parse body.
      int contentLength(getContentLength());
      if (contentLength > MAX_BODY_LENGTH)
      {
         DEBUG_ARDUINO_HTTP_SERVER_PRINTLN("Content-Length larger then the maximum content we can consume. Trunkating body.");
         contentLength = MAX_BODY_LENGTH;
      }

      DEBUG_ARDUINO_HTTP_SERVER_PRINT("Content-Length: ");
      DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(contentLength);

      if(contentLength > 0)
      {
         DEBUG_ARDUINO_HTTP_SERVER_PRINT("Parsing body .... ");
         m_stream.readBytes(m_body, contentLength);
         DEBUG_ARDUINO_HTTP_SERVER_PRINTLN("done");
      }
   }

   return m_result == ResultOk;
}


//------------------------------------------------------------------------------
//! \brief Read a single line from Stream into _linebuffer_
template <size_t MAX_BODY_LENGTH>
bool ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::readLine(char linebuffer[])
{
    if(m_result!=ResultOk) { return false; }

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
template <size_t MAX_BODY_LENGTH>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::parseRequest(char lineBuffer[])
{
    parseMethod(lineBuffer);
    parseResource();
    parseVersion();
}

//------------------------------------------------------------------------------
//! \brief Parse method: GET, PUT, HEAD, etc.
template <size_t MAX_BODY_LENGTH>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::parseMethod(char lineBuffer[])
{
    if(m_result!=ResultOk) { return; }

    // First strtok call, initialize with cached line buffer.
    String token(strtok_r(lineBuffer, " ", &m_lineBufferStrTokContext));

   if(token == "GET")
   {
      m_method = MethodGet;
   }
   else if (token == "PUT")
   {
      m_method = MethodPut;
   }
   else if (token == "POST")
   {
      m_method = MethodPost;
   }
   else if (token == "HEAD")
   {
      m_method = MethodHead;
   }
   else
   {
      m_method = MethodInvalid;
      setError(String("Cannot handle HTTP method: \"")+token+"\".");
   }
}

//! Parse "HTTP/1.1" (or any other version).
template <size_t MAX_BODY_LENGTH>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::parseVersion()
{
    if(m_result!=ResultOk) { return; }

    String version(strtok_r(0, " ", &m_lineBufferStrTokContext));
    int slashPosition = version.lastIndexOf('/');

    // String returns unsigned int for length.
    if (static_cast<unsigned int>(slashPosition) < version.length() && slashPosition > 0)
    {
        m_version = HttpVersion(version.substring(slashPosition));
    }
    else
    {
        String message("Parse error. Invalid HTTP version: \"");
        message += version;
        message += "\".";
        setError(message);

    }

}

template <size_t MAX_BODY_LENGTH>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::parseResource()
{
   if(m_result!=ResultOk) { return; }

    String resource( strtok_r(0, " ", &m_lineBufferStrTokContext) );
    m_resource = ArduinoHttpServer::HttpResource(resource);

    if (!m_resource.isValid())
    {
        setError("Parse error. No resource specified.");
    }
}

template <size_t MAX_BODY_LENGTH>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::neglectToken()
{
    strtok_r(0, " ", &m_lineBufferStrTokContext);
}

template <size_t MAX_BODY_LENGTH>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::parseField(char lineBuffer[])
{
   if(m_result!=ResultOk) { return; }

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

template <size_t MAX_BODY_LENGTH>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_LENGTH>::setError(const String& errorMessage)
{
   m_result = ResultError;
   m_errorDescription = errorMessage;
}

#endif // __ArduinoHttpServer__StreamHttpRequest__
