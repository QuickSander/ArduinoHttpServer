//
//! \file
//  WifiESP8266SmartSwitch
//
//  Created by Sander van Woensel on 24-02-15.
//  Copyright (c) 2016 Sander van Woensel. All rights reserved.
//

#ifndef __ArduinoHttpServer__HttpResource__
#define __ArduinoHttpServer__HttpResource__

#include <WString.h>

namespace ArduinoHttpServer
{

//! The resource requested by a client.
class HttpResource
{

public:
    HttpResource(const String& resource);
    HttpResource();

    HttpResource& operator=(const HttpResource& other);

    bool isValid();
    String operator[](const unsigned int index) const;
    const String& toString() const;

private:
   static const char RESOURCE_SEPERATOR = '/';

   String m_resource;

};

}

#endif // defined(__ArduinoHttpServer__HttpResource__)
