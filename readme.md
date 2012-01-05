# Webduino

### Project originally forked from 1.4.1 of http://code.google.com/p/webduino/ by Ben Combee

This is an Arduino-based Web Server library, originally developed for a class at NYC Resistor. It's called Webduino, and it's an extensible web server library for the Arduino using the Wiznet-based Ethernet shields. It's released under the MIT license allowing all sorts of reuse.

I've got a few examples up right now -- the Buzz example interfaces with Zach's Danger Shield allowing you to remotely start and stop the speaker. We had a room full of students pinging my Arduino board on Saturday, buzzing me while I was lecturing. It was fun. The big Demo example shows how to setup an HTML form running on the device where you can read and write pins.

I hope to add a few more examples in the next few weeks, including a web-enabled fridge sign using a serial LCD and how to serve things other than just HTML text from the device. I also need to work on more documentation and finishing up my slides.

## Features

- URL parameter parsing
- Handle HTTP Methods, e.g. GET, POST
- Web Forms
- Images
- JSON/RESTful interface
- HTTP Basic Authentication

## Installation Notes

With Arduino 1.0, add the Webduino folder to the "libraries" folder of your sketchbook directory.

You can put the examples in your own sketchbook directory, or in hardware/libraries/webduino/examples, as you prefer.

If you get an error message when building the examples similar to "WebServer.h not found", it's a problem with where you put the Webduino folder. The server won't work if the header is directly in the libraries folder.

## Presentation

[Wedbuino Presentation on Google Docs](http://docs.google.com/present/view?id=dd8gqxt8_5c8w9qfg3)

## Compatible Ethernet Shields

These have all been tested with the library successfully:

- Freetronics Etherten
- Freetronics Ethernet Shield
- Arduino Ethernet Shield, both original and updated microSD version
- Adafruit Ethernet Shield w/ Wiznet 811MJ module
- NKC Electronics Ethernet Shield DIY Kit

## Version history

### 1.6 released in Jan 2012

- added checkCredentials and httpUnauthorized methods as well as readHeader method for HTTP Basic Authentication; currently users will have to do the Base64 encoding outside of Webduino and I'm uncertain whether I ever want this inside the library or not...
- fixed the request parser: previously the command dispatcher would always pass true for tail_complete, even if the tail was incomplete
- fixed the command dispatcher: previously the default command could not have a tail, but the EPIC FAIL was returned

### 1.5 released in Dec 2011

- added a default favicon.ico based on the led.png from the Image example to save resources on Firefox trying to load this file on every request if it can't be loaded
- added keywords.txt for syntax highlighting in Arduino IDE
- bumped the version number up in response headers and compiler variables
- added version history to readme
- fixed default value for prefix
- fixed /index.html in Hello World example

### releases between Jul 2011 and Dec 2011

- project forked on GitHub by Chris Lee
- JSON/RESTful interface
- header Access-Control-Allow-Origin added
- code split in .h and .cpp files
- Arduino 1.0 changes by Dave Falkenburg and others
- adding CRLF after extraHeaders

### 1.4.1 released in Nov 2009

- fix examples to use readPOSTparam method

### 1.4 released in Sep 2009

- bug fix for multple connections

### 1.3.1 released in Aug 2009

- major bug fix for 1.3 for posts where Content-Length is last header sent

### 1.3 released in Aug 2009

- mostly robustness fixes (beta)

### 1.2.1 released in Aug 2009

- fixed HelloWorld example

### 1.2 released in Jul 2009

- now with GET parameter handling

### 1.1 released in Apr 2009

### 1.0 released in Mar 2009
