//
//! \file
//  ArduinoHttpServer
//
//  Created by Sander van Woensel on 23-01-16.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//
//! HTTP reply printed to Stream.

#ifndef __ArduinoHttpServer__StreamHttpReply__
#define __ArduinoHttpServer__StreamHttpReply__

#include <Arduino.h>

#include "ArduinoHttpServerDebug.h"

namespace ArduinoHttpServer
{

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! Shared Reply functionality
class AbstractStreamHttpReply
{

public:
    virtual void sendHeader(size_t size, const String& title);
    virtual void send(const String& data, const String& title);
    virtual void send(const uint8_t* buf, const size_t size, const String& title);

protected:
   AbstractStreamHttpReply(Stream& stream, const String& contentType, const String& code);
   virtual Stream& getStream();
   virtual const String& getCode();
   virtual const String& getContentType();

   constexpr static const char* CONTENT_TYPE_TEXT_HTML PROGMEM = "text/html";
   constexpr static const char* CONTENT_TYPE_APPLICATION_JSON PROGMEM = "application/json";

private:

   Stream& m_stream;
   String m_contentType; //!< Needs to be overridden to default when required. Therefore not const.
   const String m_code;

};

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! Error reply
class StreamHttpErrorReply: public AbstractStreamHttpReply
{
public:
    StreamHttpErrorReply(Stream& stream, const String& contentType, const String& code = "400");
    virtual void send(const String& data);

protected:
    virtual String getHtmlBody(const String& data);
    virtual String getJsonBody(const String& data);
};

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! Authenticate reply
#ifndef ARDUINO_HTTP_SERVER_NO_BASIC_AUTH
class StreamHttpAuthenticateReply: public AbstractStreamHttpReply
{
public:
    StreamHttpAuthenticateReply(Stream& stream, const String& contentType);
    virtual void send();
};
#endif

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! Normal reply.
class StreamHttpReply: public AbstractStreamHttpReply
{
public:
    StreamHttpReply(Stream& stream, const String& contentType);
    virtual void send(const String& data, const bool gzipencoded=false) { AbstractStreamHttpReply::send(data, "OK"); };
    virtual void send(const uint8_t* buf, const size_t size, const String& title="OK") { AbstractStreamHttpReply::send(buf, size, title); };
    virtual void sendHeader(size_t size, const String& title="OK") { AbstractStreamHttpReply::sendHeader(size, title); }
};


}

#endif // __ArduinoHttpServer__StreamHttpReply__
