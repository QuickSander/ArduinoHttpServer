//
//  StreamHttpRequest.h
//  WifiPhotoCellSensor
//
//  Created by Sander van Woensel on 24-02-15.
//  Copyright (c) 2015 Sander van Woensel. All rights reserved.
//

#ifndef __ArduinoHttpServer__StreamHttpRequest__
#define __ArduinoHttpServer__StreamHttpRequest__

#include "internals/HttpResource.h"
#include "internals/HttpField.h"

#include <Arduino.h>

namespace ArduinoHttpServer
{

//! HTTP request read from a _Stream_.
class StreamHttpRequest
{

public:
    enum MethodEnum {METHOD_INVALID, METHOD_GET, METHOD_PUT, METHOD_POST};


    StreamHttpRequest(Stream& stream);

    ~StreamHttpRequest();

    bool readRequest();

    // Header retrieval methods.
    const ArduinoHttpServer::HttpResource& getResource() const;
    const MethodEnum getMethod();

    // Field retrieval methods.
    const String& getContentType();
    const int getContentLength();

    // Body retrieval methods.
    const char * const getBody();

    // State retrieval
    const String& getErrorDescrition();
    Stream& getStream() { return m_stream; };

private:

    void parseRequest(char lineBuffer[]);
    void parseMethod(char lineBuffer[]);
    void parseResource();
    void parseVersion();

    void parseField(char lineBuffer[]);

    void neglectToken();

    bool readLine(char lineBuffer[]);

    void setError(const String& errorMessage);


    enum ResultEnum {RESULT_ERROR, RESULT_OK};

    static const int MAX_LINE_SIZE = 255+1;
    static const int MAX_BODY_LENGTH = 1023;
    static const int MAX_BODY_SIZE = MAX_BODY_LENGTH+1; //!< Byte size of array. Leaves space for terminating \0.
    static const long LINE_READ_TIMEOUT_MS = 10000L; //!< [ms] Wait 10s for reception of a complete line.
    static const int MAX_RETRIES_WAIT_DATA_AVAILABLE = 255;

    Stream& m_stream;

    char m_currentLine[MAX_LINE_SIZE]; //!< \todo This can become a local variable.
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

#endif /* defined(__ArduinoHttpServer__StreamHttpRequest__) */
