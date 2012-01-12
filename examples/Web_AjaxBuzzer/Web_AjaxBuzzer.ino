/* Web_Buzzer.pde - example sketch for Webduino library */

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

// CHANGE THIS TO YOUR OWN UNIQUE VALUE
static uint8_t mac[6] = { 0x02, 0xAA, 0xBB, 0xCC, 0x00, 0x22 };

// CHANGE THIS TO MATCH YOUR HOST NETWORK
static uint8_t ip[4] = { 192, 168, 1, 210 }; // area 51!

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
void buzzCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
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
"<!DOCTYPE html><html><head>"
  "<title>Webduino AJAX Buzzer Example</title>"
  "<link href='http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/themes/base/jquery-ui.css' rel=stylesheet />"
  //"<meta http-equiv='Content-Script-Type' content='text/javascript'>"
  "<script src='http://ajax.googleapis.com/ajax/libs/jquery/1.6.4/jquery.min.js'></script>"
  "<script src='http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/jquery-ui.min.js'></script>"
  "<style> #slider { margin: 10px; } </style>"
  "<script>"
    "function changeBuzz(event, ui) { $('#indicator').text(ui.value); $.post('/buzz', { buzz: ui.value } ); }"
    "$(document).ready(function(){ $('#slider').slider({min: 0, max:8000, change:changeBuzz}); });"
  "</script>"
"</head>"
"<body style='font-size:62.5%;'>"
  "<h1>Test the Buzzer!</h1>"
  "<div id=slider></div>"
  "<p id=indicator>0</p>"
"</body>"
"</html>";

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
