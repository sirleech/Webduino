/* Web_AjaxRGB_mobile.pde - example sketch for Webduino library */
/* -  offers web-based slider controllers for RGB led  - */

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
  "<meta charset=\"utf-8\"><meta name=\"apple-mobile-web-app-capable\" content=\"yes\" /><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge,chrome=1\"><meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">"
  "<title>Webduino RGB</title>"
  "<link rel=\"stylesheet\" href=\"http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.css\" />"
  "<script src=\"http://code.jquery.com/jquery-1.6.4.min.js\"></script>"
  "<script src=\"http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.js\"></script>"
  "<style> body, .ui-page { background: black; } .ui-body { padding-bottom: 1.5em; } div.ui-slider { width: 88%; } #red, #green, #blue { display: block; margin: 10px; } #red { background: #f00; } #green { background: #0f0; } #blue { background: #00f; } </style>"
  "<script>"
// causes the Arduino to hang quite frequently (more often than Web_AjaxRGB.pde), probably due to the different event triggering the ajax requests
    "$(document).ready(function(){ $('#red, #green, #blue').slider; $('#red, #green, #blue').bind( 'change', function(event, ui) { jQuery.ajaxSetup({timeout: 110}); /*not to DDoS the Arduino, you might have to change this to some threshold value that fits your setup*/ var id = $(this).attr('id'); var strength = $(this).val(); if (id == 'red') $.post('/rgb', { red: strength } ); if (id == 'green') $.post('/rgb', { green: strength } ); if (id == 'blue') $.post('/rgb', { blue: strength } ); });});"
  "</script>"
"</head>"
"<body>"
  "<div data-role=\"header\" data-position=\"inline\"><h1>Webduino RGB</h1></div>"
    "<div class=\"ui-body ui-body-a\">"
      "<input type=\"range\" name=\"slider\" id=\"red\" value=\"0\" min=\"0\" max=\"255\"  />"
      "<input type=\"range\" name=\"slider\" id=\"green\" value=\"0\" min=\"0\" max=\"255\"  />"
      "<input type=\"range\" name=\"slider\" id=\"blue\" value=\"0\" min=\"0\" max=\"255\"  />"
    "</div>"
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
