/* Web_Buzzer.pde - example sketch for Webduino library */

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

/* all URLs on this server will start with /buzz because of how we
 * define the PREFIX value.  We also will listen on port 80, the
 * standard HTTP service port */
#define PREFIX "/buzz"
WebServer webserver(PREFIX, 80);

/* the piezo speaker on the Danger Shield is on PWM output pin #3 */
#define BUZZER_PIN 3

/* this is the number of microseconds to wait after turning the
 * speaker on before turning it off. */
int buzzDelay = 0;

/* toggle is used to only turn on the speaker every other loop
iteration. */
char toggle = 0;

/* This command is set as the default command for the server.  It
 * handles both GET and POST requests.  For a GET, it returns a simple
 * page with some buttons.  For a POST, it saves the value posted to
 * the buzzDelay variable, affecting the output of the speaker */
void buzzCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);

      /* this is a standard string comparison function.  It returns 0
       * when there's an exact match.  We're looking for a parameter
       * named "buzz" here. */
      if (strcmp(name, "buzz") == 0)
      {
	/* use the STRing TO Unsigned Long function to turn the string
	 * version of the delay number into our integer buzzDelay
	 * variable */
        buzzDelay = strtoul(value, NULL, 10);
      }
    } while (repeat);
    
    // after procesing the POST data, tell the web browser to reload
    // the page using a GET method. 
    server.httpSeeOther(PREFIX);
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    /* store the HTML in program memory using the P macro */
    P(message) = 
      "<html><head><title>Webduino Buzzer Example</title>"
      "<body>"
      "<h1>Test the Buzzer!</h1>"
      "<form action='/buzz' method='POST'>"
      "<p><button name='buzz' value='0'>Turn if Off!</button></p>"
      "<p><button name='buzz' value='500'>500</button></p>"
      "<p><button name='buzz' value='1975'>1975</button></p>"
      "<p><button name='buzz' value='3000'>3000</button></p>"
      "<p><button name='buzz' value='8000'>8000</button></p>"
      "</form></body></html>";

    server.printP(message);
  }
}

void setup()
{
  // set the PWM output for the buzzer to out
  pinMode(BUZZER_PIN, OUTPUT);

  // setup the Ehternet library to talk to the Wiznet board
  Ethernet.begin(mac, ip);

  /* register our default command (activated with the request of
   * http://x.x.x.x/buzz */
  webserver.setDefaultCommand(&buzzCmd);

  /* start the server to wait for connections */
  webserver.begin();
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();

  /* every other time through the loop, turn on and off the speaker if
   * our delay isn't set to 0. */
  if ((++toggle & 1) && (buzzDelay > 0))
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(buzzDelay);
    digitalWrite(BUZZER_PIN, LOW);
  }
}
