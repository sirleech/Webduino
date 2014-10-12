//
//  ___FILENAME___
//
//  Project ___PROJECTNAME___
//  Created by ___FULLUSERNAME___ on ___DATE___
//  Copyright (c) ___YEAR___ ___ORGANIZATIONNAME___
//

/* NOTES: For code completion to work:
    1. Select the Project
    2. Select the target named "Index"
    3. Select Build Phase
    4. Expand it; hit plus at the bottom 
    5. Add all *.c, *.cpp files to it
 

 To Make/Compile, select the target Make.
 To Upload, select the target Upload.
*/

#include "HelloWorld.h"
#include "Wire.h"
#include <stdint.h>

#define WEBDUINO_FAVICON_DATA ""
#include "WebServer.h"


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
        const char *helloMsg = "<html><body><h1>Hello, World!</h1></body></html>";

        /* this is a special form of print that outputs from PROGMEM */
        server.print(helloMsg);
    }
}

void setup() {
#if DEBUG
    Serial.begin(9600);
    Serial.println("Hello world.");
#endif
    
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