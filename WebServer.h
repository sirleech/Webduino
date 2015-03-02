/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil;  c-file-style: "k&r"; c-basic-offset: 2; -*-
 
 Webduino, a simple Arduino web server
 Copyright 2009-2014 Ben Combee, Ran Talbott, Christopher Lee, Martin Lormes
 Francisco M Cuenca-Acuna
 
 Port to using CC3000 library: Copyright 2014 by Corbin Dunn
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef WEBDUINO_H_
#define WEBDUINO_H_

#include <string.h>
#include <stdlib.h>
#include "Print.h"

// Pick one below:
#define USE_ETHERNET_LIBRARY 0
#define USE_CC3000_LIBRARY 1

#if USE_ETHERNET_LIBRARY

#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>

#endif


#if USE_CC3000_LIBRARY

#include "Adafruit_CC3000.h"
#include "Adafruit_CC3000_Server.h"
#include "CC3000_MDNS.h"


// These are the pins for the CC3000 chip if you are using a breakout board
#define ADAFRUIT_CC3000_IRQ   5  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  6
#define ADAFRUIT_CC3000_CS    7

#else
 No implementation
#endif

#ifndef WEBDUINO_COMMANDS_COUNT
#define WEBDUINO_COMMANDS_COUNT 14
#endif

#ifndef WEBDUINO_OUTPUT_BUFFER_SIZE
#define WEBDUINO_OUTPUT_BUFFER_SIZE 32
#endif

// requires a buffer for the content type
#ifndef WEBDUINO_SUPPORT_CONTENT_TYPE
#define WEBDUINO_SUPPORT_CONTENT_TYPE 1
#endif

/* Return codes from nextURLparam.  NOTE: URLPARAM_EOS is returned
 * when you call nextURLparam AFTER the last parameter is read.  The
 * last actual parameter gets an "OK" return code. */

enum URLPARAM_RESULT { URLPARAM_OK,
    URLPARAM_NAME_OFLO,
    URLPARAM_VALUE_OFLO,
    URLPARAM_BOTH_OFLO,
    URLPARAM_EOS         // No params left
};

class WebServer: public Print
{
public:
    // passed to a command to indicate what kind of request was received
    enum ConnectionType { INVALID, GET, HEAD, POST, PUT, DELETE, PATCH };
    
    // any commands registered with the web server have to follow
    // this prototype.
    // url_tail contains the part of the URL that wasn't matched against
    //          the registered command table.
    // tail_complete is true if the complete URL fit in url_tail,  false if
    //          part of it was lost because the buffer was too small.
    typedef void Command(WebServer &server, ConnectionType type,
                         char *url_tail, bool tail_complete);

    // Prototype for the optional function which consumes the URL path itself.
    // url_path contains pointers to the seperate parts of the URL path where '/'
    //          was used as the delimiter.
    typedef void UrlPathCommand(WebServer &server, ConnectionType type,
                                char **url_path, char *url_tail,
                                bool tail_complete);
    
    // constructor for webserver object
    WebServer(const char *urlPrefix = "", uint16_t port = 80);
    
    // start listening for connections
    // NOTE: w/ the Wifi connection this doesn't start until wifi is connected
    void begin();
    
    // check for an incoming connection, and if it exists, process it
    // by reading its request and calling the appropriate command
    // handler.  This version is for compatibility with apps written for
    // version 1.1,  and allocates the URL "tail" buffer internally.
    void processConnection();
    
    // check for an incoming connection, and if it exists, process it
    // by reading its request and calling the appropriate command
    // handler.  This version saves the "tail" of the URL in buff.
    void processConnection(char *buff, int *bufflen);
    
    // set command that's run when you access the root of the server
    void setDefaultCommand(Command *cmd);
    
    // set command run for undefined pages
    void setFailureCommand(Command *cmd);
    
    // add a new command to be run at the URL specified by verb
    void addCommand(const char *verb, Command *cmd);
    
    // Set command that's run if default command or URL specified commands do
    // not run, uses extra url_path parameter to allow resolving the URL in the
    // function.
    void setUrlPathCommand(UrlPathCommand *cmd);
    
    // utility function to output CRLF pair
    void printCRLF();
    
    // output a string stored in program memory, usually one defined
    // with the P macro
    void printP(const unsigned char *str);
    
    // inline overload for printP to handle signed char strings
    void printP(const char *str) {
        printP((unsigned char*)str);
    }
    
    // support for C style formating
    void printf(char *fmt, ... );
#ifdef F
    void printf(const __FlashStringHelper *format, ... );
#endif
    
    // output raw data stored in program memory
    void writeP(const unsigned char *data, size_t length);
    
    // output HTML for a radio button
    void radioButton(const char *name, const char *val,
                     const char *label, bool selected);
    
    // output HTML for a checkbox
    void checkBox(const char *name, const char *val,
                  const char *label, bool selected);
    
    // returns next character or -1 if we're at end-of-stream
    int read();
    
    // put a character that's been read back into the input pool
    void push(int ch);
    
    // returns true if the string is next in the stream.  Doesn't
    // consume any character if false, so can be used to try out
    // different expected values.
    bool expect(const char *expectedStr);
    
    // returns true if a number, with possible whitespace in front, was
    // read from the server stream.  number will be set with the new
    // value or 0 if nothing was read.
    bool readInt(int &number);
    
    // reads a header value, stripped of possible whitespace in front,
    // from the server stream
    void readHeader(char *value, int valueLen);
    
    // Read the next keyword parameter from the socket.  Assumes that other
    // code has already skipped over the headers,  and the next thing to
    // be read will be the start of a keyword.
    //
    // returns true if we're not at end-of-stream
    bool readPOSTparam(char *name, int nameLen, char *value, int valueLen);
    
    // Read the next keyword parameter from the buffer filled by getRequest.
    //
    // returns 0 if everything weent okay,  non-zero if not
    // (see the typedef for codes)
    URLPARAM_RESULT nextURLparam(char **tail, char *name, int nameLen,
                                 char *value, int valueLen);
    
    
    typedef void ContentTypeHandler(WebServer &server, char *contentType, char *name, char *filename);

    char *getContentType(); // Returns the Content-Type: result, if available, else NULL
    bool skipToNextMultipartBoundaryStart();
    bool readMultipartFormDataName(char *name, int nameLength, char *filename = NULL, int filenameLength = 0);
    bool readMultipartFormDataContent(char *buffer, int bufferLen, int &amountRead); // returns true when done!!

    // compare string against credentials in current request
    //
    // authCredentials must be Base64 encoded outside of Webduino
    // (I wanted to be easy on the resources)
    //
    // returns true if strings match, false otherwise
    bool checkCredentials(const char authCredentials[45]);
    
    
    // output headers and a message indicating a server error
    void httpFail();
    
    // output headers and a message indicating "401 Unauthorized"
    void httpUnauthorized();
    
    // output headers and a message indicating "500 Internal Server Error"
    void httpServerError();
    
    // output headers indicating "204 No Content" and no further message
    void httpNoContent();
    
    // output standard headers indicating "200 Success".  You can change the
    // type of the data you're outputting or also add extra headers like
    // "Refresh: 1".  Extra headers should each be terminated with CRLF.
    void httpSuccess(const char *contentType = "text/html; charset=utf-8",
                     const char *extraHeaders = NULL);
    
    // used with POST to output a redirect to another URL.  This is
    // preferable to outputting HTML from a post because you can then
    // refresh the page without getting a "resubmit form" dialog.
    void httpSeeOther(const char *otherURL);
    
    // implementation of write used to implement Print interface
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    
    // tells if there is anything to process
    uint8_t available();
    
    // Flush the send buffer
    void flushBuf();
    
    // Close the current connection and flush ethernet buffers
    void reset();
    
#if USE_CC3000_LIBRARY
    Adafruit_CC3000 *getWifiManager(); // Allows setup of the network or direct access
    Client *getClient(); // Allows reading the rest of the data till connection close
#endif
    
private:
#if USE_ETHERNET_LIBRARY
    EthernetServer m_server;
    EthernetClient m_client;
#endif
#if USE_CC3000_LIBRARY
    // TODO: should I make it take an instance and have it be managed seperately? Might be better if someone wants more than a webserver
    
    Adafruit_CC3000 m_cc3000; // = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIV2);
    
    Adafruit_CC3000_Server m_server;
    Adafruit_CC3000_ClientRef m_client;
    MDNSResponder m_mdns;
    bool m_didBegin;
#endif
    
    
    // Call on each loop
    void process();
    
    const char *m_urlPrefix;
    
    unsigned char m_pushback[32];
    unsigned char m_pushbackDepth;
    
    int m_contentLength;
    char m_authCredentials[51];
    bool m_readingContent;
    
    Command *m_failureCmd;
    Command *m_defaultCmd;
    struct CommandMap
    {
        const char *verb;
        Command *cmd;
    } m_commands[WEBDUINO_COMMANDS_COUNT];
    unsigned char m_cmdCount;
    UrlPathCommand *m_urlPathCmd;
    
    uint8_t m_buffer[WEBDUINO_OUTPUT_BUFFER_SIZE];
    uint8_t m_bufFill;
    
#if WEBDUINO_SUPPORT_CONTENT_TYPE
#define WEBDUINO_CONTENT_TYPE_BUFFER_SIZE 32 // rather abitrary; must be big enough to store the content-type NAME you expect and boundary length (ie: multipart/form-data, but not the actual data)
    char m_contentType[WEBDUINO_CONTENT_TYPE_BUFFER_SIZE];
    char m_contentBoundary[WEBDUINO_CONTENT_TYPE_BUFFER_SIZE];
    void readContentType(bool includeBoundary);
    bool readMultipartFormDataParameters(char *name, int nameLen, char *value, int valueLen);
#endif
    
    void getRequest(WebServer::ConnectionType &type, char *request, int *length);
    bool dispatchCommand(ConnectionType requestType, char *verb,
                         bool tail_complete);
    void processHeaders();
    void outputCheckboxOrRadio(const char *element, const char *name,
                               const char *val, const char *label,
                               bool selected);
    
    static void defaultFailCmd(WebServer &server, ConnectionType type,
                               char *url_tail, bool tail_complete);
    void noRobots(ConnectionType type);
    void favicon(ConnectionType type);
};


#endif // WEBDUINO_H_
