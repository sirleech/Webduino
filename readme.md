# Webduino
### Project forked from 1.4.1 of http://code.google.com/p/webduino/ By Ben Combee

This is an Arduino-based Web Server library, originally developed for a class at NYC Resistor. It's called Webduino, and it's an extensible web server library for the Arduino using the Wiznet-based Ethernet shields. It's released under the MIT license allowing all sorts of reuse. 

I've got a few examples up right now -- the Buzz example interfaces with Zach's Danger Shield allowing you to remotely start and stop the speaker. We had a room full of students pinging my Arduino board on Saturday, buzzing me while I was lecturing. It was fun. The big Demo example shows how to setup an HTML form running on the device where you can read and write pins.

I hope to add a few more examples in the next few weeks, including a web-enabled fridge sign using a serial LCD and how to serve things other than just HTML text from the device. I also need to work on more documentation and finishing up my slides.

### Features
- URL parameter parsing
- Handle HTTP Methods, e.g. GET, POST
- Web Forms
- Images
- JSON/RESTful interface

### Installation Notes

With Arduino 0022, add the Webduino folder to the "libraries" folder of your sketchbook directory. 

You can put the examples in your own sketchbook directory, or in hardware/libraries/webduino/examples, as you prefer.

If you get an error message when building the examples similar to "WebServer.h not found", it's a problem with where you put the Webduino folder. The server won't work if the header is directly in the libraries folder.


### Presentation
[Wedbuino Presentation on Google Docs](http://docs.google.com/present/view?id=dd8gqxt8_5c8w9qfg3)

### Compatible Ethernet Shields
These have all been tested with the library successfully:

- Freetronics Etherten
- Freetronics Ethernet Shield
- Arduino Ethernet Shield, both original and updated microSD version
- Adafruit Ethernet Shield w/ Wiznet 811MJ module
- NKC Electronics Ethernet Shield DIY Kit


