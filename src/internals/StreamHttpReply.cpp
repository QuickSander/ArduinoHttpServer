#include "StreamHttpReply.hpp"

#include <Arduino.h>

#include "ArduinoHttpServerDebug.h"

//------------------------------------------------------------------------------
//                             Class Definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief Constructor.
ArduinoHttpServer::AbstractStreamHttpReply::AbstractStreamHttpReply(Stream& stream, const String& contentType, const String& code) :
   m_stream(stream),
   m_contentType(contentType),
   m_code(code)
{

}

//------------------------------------------------------------------------------
//! \brief Send this reply / print this reply to stream.
//! \todo: Accept char* also for data coming directly from flash.
void ArduinoHttpServer::AbstractStreamHttpReply::send(const String& data, const String& title)
{
   // Read away remaining bytes.
   while(getStream().read()>=0);

   String httpErrorReply;
   httpErrorReply += AHS_F("HTTP/1.1 ");
   httpErrorReply += getCode() + " ";
   httpErrorReply += title + "\r\n";
   httpErrorReply += AHS_F("Connection: close\r\n");
   httpErrorReply += AHS_F("Content-Length: ");
   httpErrorReply += data.length(); httpErrorReply += AHS_F("\r\n");
   httpErrorReply += AHS_F("Content-Type: "); httpErrorReply += m_contentType; httpErrorReply+= AHS_F("\r\n");
   httpErrorReply += AHS_F("\r\n");
   httpErrorReply += data;
   httpErrorReply += AHS_F("\r\n");

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
      m_contentType = CONTENT_TYPE_TEXT_HTML; // Default content type.
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

   if(getContentType() == CONTENT_TYPE_TEXT_HTML)
   {
      body = getHtmlBody(data);
   }
   else if(getContentType() == CONTENT_TYPE_APPLICATION_JSON)
   {
      body = getJsonBody(data);
   }
   else
   {
      body = data;
   }

   AbstractStreamHttpReply::send(body, data);
}


String ArduinoHttpServer::StreamHttpErrorReply::getHtmlBody(const String& data)
{
   String body;
   body += AHS_F("<html><head><title>Error: ");
   body += getCode();
   body += AHS_F("</title></head><body><h3>Error ");
   body += getCode();
   body += AHS_F(": ");
   body += data;
   body += AHS_F("</h3></body></html>");

   return body;
}

String ArduinoHttpServer::StreamHttpErrorReply::getJsonBody(const String& data)
{
   // Copy string since replace modifies original.
   String dataCopy(data);
   dataCopy.replace("\"", "\\\"");

   String body;
   body += AHS_F("{\"Error\": \"");
   body +=dataCopy;
   body += AHS_F("\"}");

   return body;
}
