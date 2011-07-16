/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil;  c-file-style: "k&r"; c-basic-offset: 2; -*-

   Webduino, a simple Arduino web server
   Copyright 2009 Ben Combee, Ran Talbott

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
#include "avr/pgmspace.h"

#include <Client.h>
#include <Server.h>

/********************************************************************
 * CONFIGURATION
 ********************************************************************/

#define WEBDUINO_VERSION 1004
#define WEBDUINO_VERSION_STRING "1.4"

#if WEBDUINO_SUPRESS_SERVER_HEADER
#define WEBDUINO_SERVER_HEADER ""
#else
#define WEBDUINO_SERVER_HEADER "Server: Webduino/" WEBDUINO_VERSION_STRING CRLF
#endif

// standard END-OF-LINE marker in HTTP
#define CRLF "\r\n"

// If processConnection is called without a buffer, it allocates one
// of 32 bytes
#define WEBDUINO_DEFAULT_REQUEST_LENGTH 32

// How long to wait before considering a connection as dead when
// reading the HTTP request.  Used to avoid DOS attacks.
#ifndef WEBDUINO_READ_TIMEOUT_IN_MS
#define WEBDUINO_READ_TIMEOUT_IN_MS 1000
#endif

#ifndef WEBDUINO_FAIL_MESSAGE
#define WEBDUINO_FAIL_MESSAGE "<h1>EPIC FAIL</h1>"
#endif

// add "#define WEBDUINO_SERIAL_DEBUGGING 1" to your application
// before including WebServer.h to have incoming requests logged to
// the serial port.
#ifndef WEBDUINO_SERIAL_DEBUGGING
#define WEBDUINO_SERIAL_DEBUGGING 0
#endif
#if WEBDUINO_SERIAL_DEBUGGING
#include <HardwareSerial.h>
#endif

// declared in wiring.h
extern "C" unsigned long millis(void);

// declare a static string
#define P(name)   static const prog_uchar name[] PROGMEM

// returns the number of elements in the array
#define SIZE(array) (sizeof(array) / sizeof(*array))

/********************************************************************
 * DECLARATIONS
 ********************************************************************/

/* Return codes from nextURLparam.  NOTE: URLPARAM_EOS is returned
 * when you call nextURLparam AFTER the last parameter is read.  The
 * last actual parameter gets an "OK" return code. */

typedef enum URLPARAM_RESULT { URLPARAM_OK,
                               URLPARAM_NAME_OFLO,
                               URLPARAM_VALUE_OFLO,
                               URLPARAM_BOTH_OFLO,
                               URLPARAM_EOS         // No params left
};

class WebServer: public Print
{
public:
  // passed to a command to indicate what kind of request was received
  enum ConnectionType { INVALID, GET, HEAD, POST };

  // any commands registered with the web server have to follow
  // this prototype.
  // url_tail contains the part of the URL that wasn't matched against
  //          the registered command table.
  // tail_complete is true if the complete URL fit in url_tail,  false if
  //          part of it was lost because the buffer was too small.
  typedef void Command(WebServer &server, ConnectionType type,
                       char *url_tail, bool tail_complete);

  // constructor for webserver object
  WebServer(const char *urlPrefix = "/", int port = 80);

  // start listening for connections
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

  // utility function to output CRLF pair
  void printCRLF();

  // output a string stored in program memory, usually one defined
  // with the P macro
  void printP(const prog_uchar *str);

  // output raw data stored in program memory
  void writeP(const prog_uchar *data, size_t length);

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

  // output headers and a message indicating a server error
  void httpFail();

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
  virtual void write(uint8_t);
  virtual void write(const char *str);
  virtual void write(const uint8_t *buffer, size_t size);
  void write(const char *data, size_t length);

private:
  Server m_server;
  Client m_client;
  const char *m_urlPrefix;

  char m_pushback[32];
  char m_pushbackDepth;

  int m_contentLength;
  bool m_readingContent;

  Command *m_failureCmd;
  Command *m_defaultCmd;
  struct CommandMap
  {
    const char *verb;
    Command *cmd;
  } m_commands[8];
  char m_cmdCount;

  void reset();
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
};

#endif // WEBDUINO_H_
