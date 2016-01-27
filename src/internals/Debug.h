
#define ARDUINO_HTTP_SERVER_DEBUG

#ifdef ARDUINO_HTTP_SERVER_DEBUG
   #define DEBUG_BEGIN(baudrate) Serial.begin(baudrate)
   #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
   #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
   #define DEBUG_BEGIN
   #define DEBUG_PRINT(...)
   #define DEBUG_PRINTLN(...)
#endif
