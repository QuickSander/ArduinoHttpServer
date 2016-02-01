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

namespace ArduinoHttpServer
{

//------------------------------------------------------------------------------
//                             Class Declaration
//------------------------------------------------------------------------------
//! Shared Reply functionality
class AbstractStreamHttpReply
{

public:
    virtual void send(const String& data, const String& title);

protected:
   AbstractStreamHttpReply(Stream& stream, const String& contentType, const String& code);
   virtual Stream& getStream();
   virtual const String& getCode();
   virtual const String& getContentType();

private:
   static const String DEFAULT_CONTENT_TYPE;

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
//! Normal reply.
class StreamHttpReply: public AbstractStreamHttpReply
{
public:
    StreamHttpReply(Stream& stream, const String& contentType);
    virtual void send(const String& data) { AbstractStreamHttpReply::send(data, "OK"); };
};


}

#endif // __ArduinoHttpServer__StreamHttpReply__
