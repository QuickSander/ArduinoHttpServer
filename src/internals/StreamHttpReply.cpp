#include "StreamHttpReply.hpp"

#include <Arduino.h>

#include "ArduinoHttpServerDebug.h"

//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief Constructor.
const String ArduinoHttpServer::AbstractStreamHttpReply::DEFAULT_CONTENT_TYPE = "text/html";

ArduinoHttpServer::AbstractStreamHttpReply::AbstractStreamHttpReply(Stream& stream, const String& contentType, const String& code) :
   m_stream(stream),
   m_contentType(contentType),
   m_code(code)
{

}

//------------------------------------------------------------------------------
//! \brief Send this reply / print this reply to stream.
void ArduinoHttpServer::AbstractStreamHttpReply::send(const String& data, const String& title)
{
   // Read away remaining bytes.
   while(getStream().read()>=0);

   String httpErrorReply;
   httpErrorReply += F("HTTP/1.1 ");
   httpErrorReply += getCode() + " ";
   httpErrorReply += title + "\r\n";
   httpErrorReply += F("Connection: close\r\n");
   httpErrorReply += F("Content-Length: ");
   httpErrorReply += data.length(); httpErrorReply += F("\r\n");
   httpErrorReply += F("Content-Type: "); httpErrorReply += m_contentType; httpErrorReply+= F("\r\n");
   httpErrorReply += F("\r\n");
   httpErrorReply += data;
   httpErrorReply += F("\r\n");

   DEBUG_ARDUINO_HTTP_SERVER_PRINT("Printing Reply ... ");
   getStream().print(httpErrorReply);
   DEBUG_ARDUINO_HTTP_SERVER_PRINTLN("done.");
}


Stream& ArduinoHttpServer::AbstractStreamHttpReply::getStream()
{
   return m_stream;
}

const String& ArduinoHttpServer::AbstractStreamHttpReply::getCode()
{
   return m_code;
}

const String& ArduinoHttpServer::AbstractStreamHttpReply::getContentType()
{
   if(m_contentType.length()<=0)
   {
      m_contentType = DEFAULT_CONTENT_TYPE;
   }
   return m_contentType;
}

//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

ArduinoHttpServer::StreamHttpReply::StreamHttpReply(Stream& stream, const String& contentType) :
   AbstractStreamHttpReply(stream, contentType, "200")
{

}


//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

ArduinoHttpServer::StreamHttpErrorReply::StreamHttpErrorReply(Stream& stream, const String& contentType, const String& code) :
   AbstractStreamHttpReply(stream, contentType, code)
{

}

void ArduinoHttpServer::StreamHttpErrorReply::send(const String& data)
{
   String body;

   if(getContentType() == "text/plain")
   {
      body = data;
   }
   else if(getContentType() == "application/json")
   {
      body = getJsonBody(data);
   }
   else
   {
      body = getHtmlBody(data);
   }

   AbstractStreamHttpReply::send(body, data);
}


String ArduinoHttpServer::StreamHttpErrorReply::getHtmlBody(const String& data)
{
   String body;
   body += F("<html><head><title>Error: ");
   body += getCode();
   body += F("</title></head><body><h3>Error ");
   body += getCode();
   body += F(": ");
   body += data;
   body += F("</h3></body></html>");

   return body;
}

String ArduinoHttpServer::StreamHttpErrorReply::getJsonBody(const String& data)
{
   // Copy string since replace modifies original.
   String dataCopy(data);
   dataCopy.replace("\"", "\\\"");

   String body;
   body += F("{\"Error\": \"");
   body +=dataCopy;
   body += F("\"}");

   return body;
}
