/* Web_Demo.pde -- sample code for Webduino server library */

/*
 * To use this demo,  enter one of the following USLs into your browser.
 * Replace "host" with the IP address assigned to the Arduino.
 *
 * http://host/
 * http://host/json
 *
 * This URL brings up a display of the values READ on digital pins 0-9
 * and analog pins 0-5.  This is done with a call to defaultCmd.
 * 
 * 
 * http://host/form
 *
 * This URL also brings up a display of the values READ on digital pins 0-9
 * and analog pins 0-5.  But it's done as a form,  by the "formCmd" function,
 * and the digital pins are shown as radio buttons you can change.
 * When you click the "Submit" button,  it does a POST that sets the
 * digital pins,  re-reads them,  and re-displays the form.
 * 
 */

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

// no-cost stream operator as described at 
// http://sundial.org/arduino/?page_id=119
template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }


// CHANGE THIS TO YOUR OWN UNIQUE VALUE
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// CHANGE THIS TO MATCH YOUR HOST NETWORK
static uint8_t ip[] = { 192, 168, 1, 210 };

#define PREFIX ""

WebServer webserver(PREFIX, 80);

// commands are functions that get called by the webserver framework
// they can read any posted data from client, and they output to server

void jsonCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    server.httpFail();
    return;
  }

  //server.httpSuccess(false, "application/json");
  server.httpSuccess("application/json");
  
  if (type == WebServer::HEAD)
    return;

  int i;    
  server << "{ ";
  for (i = 0; i <= 9; ++i)
  {
    // ignore the pins we use to talk to the Ethernet chip
    int val = digitalRead(i);
    server << "\"d" << i << "\": " << val << ", ";
  }

  for (i = 0; i <= 5; ++i)
  {
    int val = analogRead(i);
    server << "\"a" << i << "\": " << val;
    if (i != 5)
      server << ", ";
  }
  
  server << " }";
}

void outputPins(WebServer &server, WebServer::ConnectionType type, bool addControls = false)
{
  P(htmlHead) =
    "<html>"
    "<head>"
    "<title>Arduino Web Server</title>"
    "<style type=\"text/css\">"
    "BODY { font-family: sans-serif }"
    "H1 { font-size: 14pt; text-decoration: underline }"
    "P  { font-size: 10pt; }"
    "</style>"
    "</head>"
    "<body>";

  int i;
  server.httpSuccess();
  server.printP(htmlHead);

  if (addControls)
    server << "<form action='" PREFIX "/form' method='post'>";

  server << "<h1>Digital Pins</h1><p>";

  for (i = 0; i <= 9; ++i)
  {
    // ignore the pins we use to talk to the Ethernet chip
    int val = digitalRead(i);
    server << "Digital " << i << ": ";
    if (addControls)
    {
      char pinName[4];
      pinName[0] = 'd';
      itoa(i, pinName + 1, 10);
      server.radioButton(pinName, "1", "On", val);
      server << " ";
      server.radioButton(pinName, "0", "Off", !val);
    }
    else
      server << (val ? "HIGH" : "LOW");

    server << "<br/>";
  }

  server << "</p><h1>Analog Pins</h1><p>";
  for (i = 0; i <= 5; ++i)
  {
    int val = analogRead(i);
    server << "Analog " << i << ": " << val << "<br/>";
  }

  server << "</p>";

  if (addControls)
    server << "<input type='submit' value='Submit'/></form>";

  server << "</body></html>";
}

void formCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      repeat = server.readPOSTparam(name, 16, value, 16);
      if (name[0] == 'd')
      {
        int pin = strtoul(name + 1, NULL, 10);
        int val = strtoul(value, NULL, 10);
        digitalWrite(pin, val);
      }
    } while (repeat);

    server.httpSeeOther(PREFIX "/form");
  }
  else
    outputPins(server, type, true);
}

void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  outputPins(server, type, false);  
}

void setup()
{
  // set pins 0-8 for digital input
  for (int i = 0; i <= 9; ++i)
    pinMode(i, INPUT);
  pinMode(9, OUTPUT);

  Ethernet.begin(mac, ip);
  webserver.begin();

  webserver.setDefaultCommand(&defaultCmd);
  webserver.addCommand("json", &jsonCmd);
  webserver.addCommand("form", &formCmd);
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();

  // if you wanted to do other work based on a connecton, it would go here
}
