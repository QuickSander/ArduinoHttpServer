
#include <ArduinoHttpServer.h>


void setup() {
	// just check to see if escaped characters are parsed correctly
   Serial.begin(115200);
   delay(100);
   Serial.println("TEST - simple");
   Serial.print("'index.php?key=value' check for key: ");
   Serial.println(ArduinoHttpServer::HttpResource("index.php?key=value").getArgument("key"));
   // + must get changed to space
   Serial.println("TEST - with space");
   Serial.print("'action_page.php?fname=with+space&lname=' check for fname: ");
   Serial.println(ArduinoHttpServer::HttpResource("action_page.php?fname=with+space&lname=").getArgument("fname"));
   Serial.println("TEST - with special char");
   Serial.print("'action_page.php?fname=email%40server.com&lname=' check for fname: ");
   Serial.println(ArduinoHttpServer::HttpResource("action_page.php?fname=email%40server.com&lname=").getArgument("fname"));
}

void loop() {
  // put your main code here, to run repeatedly:

}