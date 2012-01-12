/* Web_HelloWorld.pde - very simple Webduino example */

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

/* CHANGE THIS TO YOUR OWN UNIQUE VALUE.  The MAC number should be
 * different from any other devices on your network or you'll have
 * problems receiving packets. */
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


/* CHANGE THIS TO MATCH YOUR HOST NETWORK.  Most home networks are in
 * the 192.168.0.XXX or 192.168.1.XXX subrange.  Pick an address
 * that's not in use and isn't going to be automatically allocated by
 * DHCP from your router. */
static uint8_t ip[] = { 192, 168, 1, 210 };

/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);

/* commands are functions that get called by the webserver framework
 * they can read any posted data from client, and they output to the
 * server to send data back to the web browser. */
void helloCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type != WebServer::HEAD)
  {
    /* this defines some HTML text in read-only memory aka PROGMEM.
     * This is needed to avoid having the string copied to our limited
     * amount of RAM. */
    P(helloMsg) = "<h1>Hello, World!</h1>";

    /* this is a special form of print that outputs from PROGMEM */
    server.printP(helloMsg);
  }
}

void setup()
{
  /* initialize the Ethernet adapter */
  Ethernet.begin(mac, ip);

  /* setup our default command that will be run when the user accesses
   * the root page on the server */
  webserver.setDefaultCommand(&helloCmd);

  /* run the same command if you try to load /index.html, a common
   * default page name */
  webserver.addCommand("index.html", &helloCmd);

  /* start the webserver */
  webserver.begin();
}

void loop()
{
  char buff[64];
  int len = 64;

  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);
}
