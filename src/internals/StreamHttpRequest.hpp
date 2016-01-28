//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 24-02-15.
//  Copyright (c) 2015 Sander van Woensel. All rights reserved.
//
//! HTTP Request read from Stream.

#ifndef __ArduinoHttpServer__StreamHttpRequest__
#define __ArduinoHttpServer__StreamHttpRequest__

#include "HttpResource.hpp"
#include "HttpField.hpp"

#include <Arduino.h>

namespace ArduinoHttpServer
{

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! HTTP request read from a _Stream_.
class StreamHttpRequest
{

public:
    enum MethodEnum {METHOD_INVALID, METHOD_GET, METHOD_PUT, METHOD_POST, METHOD_HEAD};

    StreamHttpRequest(Stream& stream);

    ~StreamHttpRequest() { };

    bool readRequest();

    // Header retrieval methods.
    inline const ArduinoHttpServer::HttpResource& getResource() const { return m_resource; };
    inline const MethodEnum getMethod() const { return m_method; };
    inline const String& getVersion() const { return m_version; };

    // Field retrieval methods.
    inline const String& getContentType() const { m_contentTypeField.getValueAsInt(); };
    inline const int getContentLength() const { m_contentLengthField.getValueAsInt(); };

    // Body retrieval methods.
    const char * const getBody();

    // State retrieval
    const String& getErrorDescrition();
    Stream& getStream() { return m_stream; };

private:

   enum ResultEnum {RESULT_ERROR, RESULT_OK};

   static const int MAX_LINE_SIZE = 255+1;
   static const int MAX_BODY_LENGTH = 1023;
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
   String m_version;

   ArduinoHttpServer::HttpField m_contentTypeField;
   ArduinoHttpServer::HttpField m_contentLengthField;

   ResultEnum m_result;
   String m_errorDescription;
};

}

#endif // __ArduinoHttpServer__StreamHttpRequest__
