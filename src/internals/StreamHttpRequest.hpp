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

#include "FixString.hpp"
#include "HttpResource.hpp"
#include "HttpField.hpp"
#include "HttpVersion.hpp"
#include "ArduinoHttpServerDebug.h"

#include <Arduino.h>
#ifndef ARDUINO_HTTP_SERVER_NO_BASIC_AUTH
   #include <Base64.h>
#endif

#include <string.h>

namespace ArduinoHttpServer
{

enum class Method : char
{
   Invalid, Get, Put, Post, Head, Delete, Options
};


typedef FixString<32> ErrorMessageString;
typedef FixString<128> ErrorString;

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! HTTP request read from a _Stream_.
template <size_t MAX_BODY_SIZE>
class StreamHttpRequest
{

public:
    StreamHttpRequest(Stream& stream);

    ~StreamHttpRequest() { };

    bool readRequest();

    // Header retrieval methods.
    inline const ArduinoHttpServer::HttpResource& getResource() const { return m_resource; };
    inline const ArduinoHttpServer::HttpVersion& getVersion() const { return m_version; };
    inline const ArduinoHttpServer::Method getMethod() const { return m_method; };

    // Field retrieval methods.
    inline const String& getContentType() const { return m_contentTypeField.getValueAsString(); };
    inline const int getContentLength() const { return m_contentLengthField.getValueAsInt(); };

    // Body retrieval methods.
    //! Retrieve zero terminated body content.
    inline const char* const getBody() const { return m_body; };

    // State retrieval
    const ErrorString getError() const;
    Stream& getStream() { return m_stream; };

    // Validate if client provided credentials match _username_ and _password_.
    #ifndef ARDUINO_HTTP_SERVER_NO_BASIC_AUTH
    bool authenticate(const char * username, const char * password) const;
    #endif

private:

   enum class Error: char {
      OK,
      TIMEOUT,
      CANNOT_HANDLE_HTTP_METHOD,
      PARSE_ERROR_INVALID_HTTP_VERSION,
      PARSE_ERROR_NO_RESOURCE
   };

   //! \todo To reduce program memory size reduce these to the proper types: char and int.
   //!    Or better yet, make these Template variables.
   static const int MAX_LINE_SIZE = 255+1;
   static const int MAX_BODY_LENGTH = MAX_BODY_SIZE-1; //!< Byte size of array. Leaves space for terminating \0.
   static const long LINE_READ_TIMEOUT_MS = 10000L; //!< [ms] Wait 10s for reception of a complete line.
   static const int MAX_RETRIES_WAIT_DATA_AVAILABLE = 255;

   void parseRequest(char lineBuffer[MAX_LINE_SIZE]);
   void parseMethod(char lineBuffer[MAX_LINE_SIZE]);
   void parseResource();
   void parseVersion();
   void parseField(char lineBuffer[MAX_LINE_SIZE]);

   void neglectToken();

   bool readLine(char lineBuffer[MAX_LINE_SIZE]);

   void setError(const Error, const ErrorMessageString& errorMessage = ErrorMessageString());

   Stream& m_stream;
   char m_body[MAX_BODY_SIZE];
   Method m_method;
   ArduinoHttpServer::HttpResource m_resource;
   ArduinoHttpServer::HttpVersion m_version;
   ArduinoHttpServer::HttpField m_contentTypeField;
   ArduinoHttpServer::HttpField m_contentLengthField;
   ArduinoHttpServer::HttpField m_authorizationField;

   Error m_error;
   ErrorMessageString m_errorDetail;

   char *m_lineBufferStrTokContext;
};

}

//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief Constructor. sets Stream timeout for reading data.
template <size_t MAX_BODY_SIZE>
ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::StreamHttpRequest(Stream& stream) :
    m_stream(stream),
    m_body{0},
    m_method(Method::Invalid),
    m_resource(),
    m_version(),
    m_contentTypeField(),
    m_contentLengthField(),
    m_error(Error::OK),
    m_errorDetail(),
    m_lineBufferStrTokContext(0)
{
   static_assert(MAX_BODY_SIZE >= 0, "HTTP body length less then zero specified.");
   m_stream.setTimeout(LINE_READ_TIMEOUT_MS);
}

//------------------------------------------------------------------------------
//! \brief Wait for data to become available on Stream and parses the request.
template <size_t MAX_BODY_SIZE>
bool ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::readRequest()
{

   char lineBuffer[MAX_LINE_SIZE] = {0};

   int attempts(0);
   while(!m_stream.available())
   {
      // Quit when failed to retrieve data after n retries.
      if(attempts >= MAX_RETRIES_WAIT_DATA_AVAILABLE)
      {
         setError(Error::TIMEOUT);
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

   return m_error == Error::OK;
}


//------------------------------------------------------------------------------
//! \brief Read a single line from Stream into _linebuffer_
template <size_t MAX_BODY_SIZE>
bool ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::readLine(char linebuffer[])
{
    if(m_error!=Error::OK) { return false; }

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
template <size_t MAX_BODY_SIZE>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::parseRequest(char lineBuffer[])
{
    parseMethod(lineBuffer);
    parseResource();
    parseVersion();
}

//------------------------------------------------------------------------------
//! \brief Parse method: GET, PUT, HEAD, etc.
template <size_t MAX_BODY_SIZE>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::parseMethod(char lineBuffer[])
{
   if(m_error!=Error::OK) { return; }

   // First strtok call, initialize with cached line buffer.
   // len("OPTIONS") + 1 for terminating null = 7
   FixString<8U> token(strtok_r(lineBuffer, " ", &m_lineBufferStrTokContext));

   if(token == "GET")
   {
      m_method = Method::Get;
   }
   else if (token == "PUT")
   {
      m_method = Method::Put;
   }
   else if (token == "POST")
   {
      m_method = Method::Post;
   }
   else if (token == "HEAD")
   {
      m_method = Method::Head;
   }
   else if (token == "DELETE")
   {
      m_method = Method::Delete;
   }
   else if (token == "OPTIONS") 
   {
      m_method = Method::Options;
   }
   else
   {
      m_method = Method::Invalid;
      setError(Error::CANNOT_HANDLE_HTTP_METHOD, token);
   }
}

//! Parse "HTTP/1.1" (or any other version).
template <size_t MAX_BODY_SIZE>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::parseVersion()
{
    if(m_error!=Error::OK) { return; }

    // HTTP/000.000
    HttpVersion::FixStringT version(strtok_r(0, " ", &m_lineBufferStrTokContext));

    //String version(strtok_r(0, " ", &m_lineBufferStrTokContext));
    int slashPosition(version.lastIndexOf('/'));

    // String returns unsigned int for length.
    if (static_cast<unsigned int>(slashPosition) < version.length() && slashPosition > 0)
    {
        m_version = HttpVersion(version.substring(slashPosition));
    }
    else
    {
        setError(Error::PARSE_ERROR_INVALID_HTTP_VERSION, version);
    }

}

template <size_t MAX_BODY_SIZE>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::parseResource()
{
   if(m_error!=Error::OK) { return; }

    String resource( strtok_r(0, " ", &m_lineBufferStrTokContext) );
    m_resource = ArduinoHttpServer::HttpResource(resource);

    if (!m_resource.isValid())
    {
        setError(Error::PARSE_ERROR_NO_RESOURCE);
    }
}

template <size_t MAX_BODY_SIZE>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::neglectToken()
{
    strtok_r(0, " ", &m_lineBufferStrTokContext);
}

template <size_t MAX_BODY_SIZE>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::parseField(char lineBuffer[])
{
   if(m_error!=Error::OK) { return; }
   
   ArduinoHttpServer::HttpField httpField(lineBuffer);

   if(httpField.getType() == ArduinoHttpServer::HttpField::Type::CONTENT_TYPE)
   {
      m_contentTypeField = httpField;
   }
   else if(httpField.getType() == ArduinoHttpServer::HttpField::Type::CONTENT_LENGTH)
   {
      m_contentLengthField = httpField;
   }
   else if(httpField.getType() == ArduinoHttpServer::HttpField::Type::AUTHORIZATION)
   {
      m_authorizationField = httpField;
   }
   else
   {
      // Ignore other fields for now.
   }
}

template <size_t MAX_BODY_SIZE>
void ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::setError(const Error error, const ErrorMessageString& errorMessage)
{
   m_error = error;
   m_errorDetail = errorMessage;
}

template <size_t MAX_BODY_SIZE>
const ArduinoHttpServer::ErrorString ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::getError() const
{
   ErrorString errorString;
   switch(m_error)
   {
      case Error::OK:
         break;

      case Error::TIMEOUT:
         errorString = AHS_F("Timeout occurred while waiting for data");
         break;

      case Error::CANNOT_HANDLE_HTTP_METHOD:
         errorString = AHS_F("Don't know how to handle HTTP method: \"");
         errorString += m_errorDetail;
         errorString += AHS_F("\"");
         break;

      case Error::PARSE_ERROR_INVALID_HTTP_VERSION:
         errorString = AHS_F("Invalid HTTP version: \"");
         errorString += m_errorDetail;
         errorString += AHS_F("\"");
         break;

      case Error::PARSE_ERROR_NO_RESOURCE:
         errorString = AHS_F("No resource specified.");
         break;

      default:
         break;
   }

   return errorString;
}

#ifndef ARDUINO_HTTP_SERVER_NO_BASIC_AUTH
template <size_t MAX_BODY_SIZE>
bool ArduinoHttpServer::StreamHttpRequest<MAX_BODY_SIZE>::authenticate(const char *username, const char *password) const
{
   if (m_authorizationField.getType() == HttpField::Type::NOT_SUPPORTED)
   {
      return false;
   }

   // HTTP value: "<Type> <Base 64 encoded credentials>"
   // Retrieve type and verify wether it is basic authorization.
   if(!(m_authorizationField.getSubValueString(0) == HttpField::BASIC_AUTH_TYPE_STR))
   {
      DEBUG_ARDUINO_HTTP_SERVER_PRINT("Unsupported authentication type: ");
      DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(m_authorizationField.getSubValueString(0).cStr());
      return false;
   }

   FixString<128U> combinedInput(username);
   combinedInput += AHS_F(":");
   combinedInput += password;

   const int encodedLength = Base64.encodedLength(combinedInput.length());

   char encodedString[encodedLength+1]; // Base64 makes sure _encodedString_ is zero terminated.
   Base64.encode(encodedString, const_cast<char*>(combinedInput.cStr()), combinedInput.length());
   
   DEBUG_ARDUINO_HTTP_SERVER_PRINT("Credentials string in client supplied auth: ");
   DEBUG_ARDUINO_HTTP_SERVER_PRINTLN(m_authorizationField.getSubValueString(1).cStr() );

   if ( m_authorizationField.getSubValueString(1) == encodedString )
   {
      return true;
   }

   return false;
}
#endif

#endif // __ArduinoHttpServer__StreamHttpRequest__
