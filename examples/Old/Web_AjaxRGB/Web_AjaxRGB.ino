/* Web_AjaxRGB.pde - example sketch for Webduino library */

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

// CHANGE THIS TO YOUR OWN UNIQUE VALUE
static uint8_t mac[6] = { 0x02, 0xAA, 0xBB, 0xCC, 0x00, 0x22 };

// CHANGE THIS TO MATCH YOUR HOST NETWORK
static uint8_t ip[4] = { 192, 168, 1, 210 }; // area 51!

/* all URLs on this server will start with /rgb because of how we
 * define the PREFIX value.  We also will listen on port 80, the
 * standard HTTP service port */
#define PREFIX "/rgb"
WebServer webserver(PREFIX, 80);

#define RED_PIN 5
#define GREEN_PIN 3
#define BLUE_PIN 6

int red = 0;            //integer for red darkness
int blue = 0;           //integer for blue darkness
int green = 0;          //integer for green darkness

/* This command is set as the default command for the server.  It
 * handles both GET and POST requests.  For a GET, it returns a simple
 * page with some buttons.  For a POST, it saves the value posted to
 * the red/green/blue variable, affecting the output of the speaker */
void rgbCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
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
       * named red/green/blue here. */
      if (strcmp(name, "red") == 0)
      {
	/* use the STRing TO Unsigned Long function to turn the string
	 * version of the color strength value into our integer red/green/blue
	 * variable */
        red = strtoul(value, NULL, 10);
      }
      if (strcmp(name, "green") == 0)
      {
        green = strtoul(value, NULL, 10);
      }
      if (strcmp(name, "blue") == 0)
      {
        blue = strtoul(value, NULL, 10);
      }
    } while (repeat);
    
    // after procesing the POST data, tell the web browser to reload
    // the page using a GET method. 
    server.httpSeeOther(PREFIX);
//    Serial.print(name);
//    Serial.println(value);

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
  "<title>Webduino AJAX RGB Example</title>"
  "<link href='http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/themes/base/jquery-ui.css' rel=stylesheet />"
  "<script src='http://ajax.googleapis.com/ajax/libs/jquery/1.6.4/jquery.min.js'></script>"
  "<script src='http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/jquery-ui.min.js'></script>"
  "<style> body { background: black; } #red, #green, #blue { margin: 10px; } #red { background: #f00; } #green { background: #0f0; } #blue { background: #00f; } </style>"
  "<script>"

// change color on mouse up, not while sliding (causes much less traffic to the Arduino):
//    "function changeRGB(event, ui) { var id = $(this).attr('id'); if (id == 'red') $.post('/rgb', { red: ui.value } ); if (id == 'green') $.post('/rgb', { green: ui.value } ); if (id == 'blue') $.post('/rgb', { blue: ui.value } ); } "
//    "$(document).ready(function(){ $('#red, #green, #blue').slider({min: 0, max:255, change:changeRGB}); });"

// change color on slide and mouse up (causes more traffic to the Arduino):
    "function changeRGB(event, ui) { jQuery.ajaxSetup({timeout: 110}); /*not to DDoS the Arduino, you might have to change this to some threshold value that fits your setup*/ var id = $(this).attr('id'); if (id == 'red') $.post('/rgb', { red: ui.value } ); if (id == 'green') $.post('/rgb', { green: ui.value } ); if (id == 'blue') $.post('/rgb', { blue: ui.value } ); } "
    "$(document).ready(function(){ $('#red, #green, #blue').slider({min: 0, max:255, change:changeRGB, slide:changeRGB}); });"

  "</script>"
"</head>"
"<body style='font-size:62.5%;'>"
  "<div id=red></div>"
  "<div id=green></div>"
  "<div id=blue></div>"
"</body>"
"</html>";

    server.printP(message);
  }
}

void setup()
{
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

//  Serial.begin(9600);

  // setup the Ehternet library to talk to the Wiznet board
  Ethernet.begin(mac, ip);

  /* register our default command (activated with the request of
   * http://x.x.x.x/rgb */
  webserver.setDefaultCommand(&rgbCmd);

  /* start the server to wait for connections */
  webserver.begin();
}

void loop()
{
  // process incoming connections one at a time forever
  webserver.processConnection();
//  Serial.print(red);
//  Serial.print(" ");
//  Serial.print(green);
//  Serial.print(" ");
//  Serial.println(blue);
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}
