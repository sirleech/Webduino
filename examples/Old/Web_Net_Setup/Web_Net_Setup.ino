/* Web_Net_Setup.pde - example for a webinterface to set the network configuration 
Author:     Matthias Maderer
Date:       07.03.2013
Version:    1.0.1
web:        www.edvler-blog.de/arduino_networksetup_webinterface_with_eeprom

This is a sample Sketch for Webduino!
More informations about Webduino can be found at https://github.com/sirleech/Webduino

For more informations about EEPROMAnything.h look at http://playground.arduino.cc/Code/EEPROMWriteAnything
*/

/*
* With this example its possible to configure the network configuration of the
* Arduino Ethernet Shield with a webinterface. Imagine like your router setup.
* 
* It's possible to configure the following network settings:
* - MAC address
* - IP address
* - Subnet
* - Gateway
* - DNS Server
* - Webserver port
* - USE DHCP YES/NO (if you use DHCP connect per serial port - 9600 baud - on powerup to see which ip address is assigned)
* - DHCP renew interval
*
* Other functions:
* - Display DHCP renew status
* - Display DHCP renew timestamp
* - Display Arduino uptime
* - Display used RAM
* 
* You can configure default settings. This settings are used wenn no configuration is present.
* Look at the function set_EEPROM_Default().
*
* It is possible to connect a RESET button. If the button is pressed and the Arduino is turned on 
* the default values would be restored too.
* See #define RESET_PIN.
*
*
* All settings are stored in EEPROM. This means they are permanent.
* Please look at http://arduino.cc/en/Reference/EEPROM for a short description.
*
* 
* To setup your arduino upload this sketch.
* 
* If you don't change the sourcecode the default IP address is http://192.168.0.111/
* Don't forget to change the IP of your network adapter to a suitable address (e.g. to IP 192.168.0.1, NETMASK 255.255.255.0)!
*
* Enter the following URL for the setup page:
* http://192.168.0.111/setupNet.html
*
* Please note that no input checks are done!!
* This means that no error would be thrown if you type a wrong IP address or other failures.
* Keep this in mind.
*
* Resources:
* There are many Strings for the HTML site. The compiled size is about 27.000 Bytes. Aprox. 2000 byte of SRAM is used.
* On smaller Arduinos this may cause problems. I've tested it on a MEGA 2560.
*
* BUGS:
* - After uploading your sketch the arduino is not reachable. --> Reset your Arduino!!
*/


#define WEBDUINO_FAVICON_DATA "" // no favicon
//#define DEBUG  //uncomment for serial debug output
#define USE_SYSTEM_LIBRARY //comment out if you want to save some space (about 1 Byte). You wouldn't see uptime and free RAM if it's commented out.
#define SERIAL_BAUD 9600


#include "SPI.h" // new include
#include "avr/pgmspace.h" // new include
#include "Ethernet.h"
#include "WebServer.h"


/* #############################################################################################################################################################
* Code for the EEPROM related things
* 
*/
#include <EEPROM.h>
#include "EEPROMAnything.h"

#define RESET_PIN 40	//Connect a button to this PIN. If the button is hold, an the device is turned on the default ethernet settings are restored.

/* structure which is stored in the eeprom. 
* Look at "EEPROMAnything.h" for the functions storing and reading the struct
*/
struct config_t
{
    byte config_set;
    byte use_dhcp;
    byte dhcp_refresh_minutes;
    byte mac[6];
    byte ip[4];
    byte gateway[4];
    byte subnet[4];
    byte dns_server[4];
    unsigned int webserverPort;
} eeprom_config;

/** 
* set_EEPROM_Default() function
*
* The default settings. 
* This settings are used when no config is present or the reset button is pressed.
*/
void set_EEPROM_Default() {
    eeprom_config.config_set=1;  // dont change! It's used to check if the config is already set
  
    eeprom_config.use_dhcp=0; // use DHCP per default
    eeprom_config.dhcp_refresh_minutes=60; // refresh the DHCP every 60 minutes
  
    // set the default MAC address. In this case its DE:AD:BE:EF:FE:ED
    eeprom_config.mac[0]=0xDE;  
    eeprom_config.mac[1]=0xAD;
    eeprom_config.mac[2]=0xBE;
    eeprom_config.mac[3]=0xEF;
    eeprom_config.mac[4]=0xFE;
    eeprom_config.mac[5]=0xED;
    
    // set the default IP address for the arduino. In this case its 192.168.0.111
    eeprom_config.ip[0]=192;
    eeprom_config.ip[1]=168;
    eeprom_config.ip[2]=0;
    eeprom_config.ip[3]=111;
  
    // set the default GATEWAY. In this case its 192.168.0.254
    eeprom_config.gateway[0]=192;
    eeprom_config.gateway[1]=168;
    eeprom_config.gateway[2]=0;
    eeprom_config.gateway[3]=254;
    
    // set the default SUBNET. In this case its 255.255.255.0
    eeprom_config.subnet[0]=255;
    eeprom_config.subnet[1]=255;
    eeprom_config.subnet[2]=255;
    eeprom_config.subnet[3]=0;

    // set the default DNS SERVER. In this case its 192.168.0.254
    eeprom_config.dns_server[0]=192;
    eeprom_config.dns_server[1]=168;
    eeprom_config.dns_server[2]=0;
    eeprom_config.dns_server[3]=254;

    // set the default Webserver Port. In this case its Port 80
    eeprom_config.webserverPort=80;
    
    #ifdef DEBUG
      Serial.println("Config reset");
    #endif 
}


/**
* read_EEPROM_Settings function
* This function is used to read the EEPROM settings at startup
*
* Overview:
* - Set the PIN for the RESET-button to input and activate pullups
* - Load the stored data from EEPROM into the eeprom_config struct
* - Check if a config is stored or the reset button is pressed. If one of the conditions is ture, set the defaults
*/
void read_EEPROM_Settings() {
  pinMode(RESET_PIN, INPUT);
  digitalWrite(RESET_PIN, HIGH);
  
  // read the current config
  EEPROM_readAnything(0, eeprom_config);
  
  // check if config is present or if reset button is pressed
  if (eeprom_config.config_set != 1 || digitalRead(RESET_PIN) == LOW) {
    // set default values
    set_EEPROM_Default();
    
    // write the config to eeprom
    EEPROM_writeAnything(0, eeprom_config);
  } 
}

/**
* print_EEPROM_Settings() function
*
* This function is used for debugging the configuration.
* It prints the actual configuration to the serial port.
*/
#ifdef DEBUG
void print_EEPROM_Settings() {
    Serial.print("IP: ");
    for(int i = 0; i<4; i++) {
      Serial.print(eeprom_config.ip[i]);
      if (i<3) {
        Serial.print('.');
      }
    }
    Serial.println();
  
    Serial.print("Subnet: ");
    for(int i = 0; i<4; i++) {
      Serial.print(eeprom_config.subnet[i]);
      if (i<3) {
        Serial.print('.');
      }
    }
    Serial.println();
    
    Serial.print("Gateway: ");
    for(int i = 0; i<4; i++) {
      Serial.print(eeprom_config.gateway[i]);
      if (i<3) {
        Serial.print('.');
      }
    }
    Serial.println();

    Serial.print("DNS Server: ");
    for(int i = 0; i<4; i++) {
      Serial.print(eeprom_config.dns_server[i]);
      if (i<3) {
        Serial.print('.');
      }
    }
    Serial.println();
    
    Serial.print("MAC: ");
    for (int a=0;a<6;a++) {
      Serial.print(eeprom_config.mac[a],HEX);
      if(a<5) {
        Serial.print(":");
      }
    }
    Serial.println();
    
    Serial.print("Webserver Port: ");
    Serial.println(eeprom_config.webserverPort);
    
    Serial.print("USE DHCP: ");
    Serial.println(eeprom_config.use_dhcp);
    
    Serial.print(" DHCP renew every ");
    Serial.print(eeprom_config.dhcp_refresh_minutes);
    Serial.println(" minutes");
    
    Serial.print("Config Set: ");
    Serial.println(eeprom_config.config_set);

}
#endif

// #############################################################################################################################################################


/* START Network section #######################################################################################################################################
* Code for setting up network connection
*/
unsigned long last_dhcp_renew;
byte dhcp_state;

/**
* renewDHCP() function
* Renew the DHCP relase in a given interval.
* 
* Overview:
* - Check if interval = 0 and set it to 1
* - Check if renew interval is reached and renew the lease
*/
void renewDHCP(int interval) {
  unsigned long interval_millis = interval * 60000;

  if (interval == 0 ) {
     interval = 1; 
  }
  if (eeprom_config.use_dhcp==1) {
    if((millis() - last_dhcp_renew) >  interval_millis) {
      last_dhcp_renew=millis();
      dhcp_state = Ethernet.maintain();
    }
  }
}


/**
* setupNetwork() function
* This function is used to setupup the network according to the values stored in the eeprom
*
* Overview:
* - First of all read the EEPROM settings
* - Display a link to the ethernet setup
* - Check if DHCP should be used, if not create instaces of IPAddress for ip, gateway, subnet and dns_server
* - Invoke Ethernet.begin with all parameters if no dhcp is active (Ethernet.begin(mac, ip, dns_server, gateway, subnet);). 
* - If DHCP is used invoke only with mac (Ethernet.begin(mac);) and display the ip on the serial console.
*/
void setupNetwork() {
  read_EEPROM_Settings();
  
  #ifdef DEBUG
   print_EEPROM_Settings();
  #endif

 // byte mac[] = { eeprom_config.mac[0], eeprom_config.mac[1], eeprom_config.mac[2], eeprom_config.mac[3], eeprom_config.mac[4], eeprom_config.mac[5] };  
  
  if (eeprom_config.use_dhcp != 1) {
    IPAddress ip(eeprom_config.ip[0], eeprom_config.ip[1], eeprom_config.ip[2], eeprom_config.ip[3]);                                               
    IPAddress gateway (eeprom_config.gateway[0],eeprom_config.gateway[1],eeprom_config.gateway[2],eeprom_config.gateway[3]);                      
    IPAddress subnet  (eeprom_config.subnet[0], eeprom_config.subnet[1], eeprom_config.subnet[2], eeprom_config.subnet[3]);  
    IPAddress dns_server  (eeprom_config.dns_server[0], eeprom_config.dns_server[1], eeprom_config.dns_server[2], eeprom_config.dns_server[3]);
    Ethernet.begin(eeprom_config.mac, ip, dns_server, gateway, subnet);
  } else {
    if (Ethernet.begin(eeprom_config.mac) == 0) {
      Serial.print("Failed to configure Ethernet using DHCP");
    }
    Serial.println(Ethernet.localIP());
  }
}
// END Network section #########################################################################################################################################


/* WEB-Server section #######################################################################################################################################
* Webserver Code
*/

#ifdef USE_SYSTEM_LIBRARY
#include "system.h"
System sys;
#endif

/* Store all string in the FLASH storage to free SRAM.
The P() is a function from Webduino.
*/
P(Page_start) = "<html><head><title>Web_EEPROM_Setup</title></head><body>\n";
P(Page_end) = "</body></html>";

P(Http400) = "HTTP 400 - BAD REQUEST";
P(Index) = "<h1>index.html</h1><br>This is your main site!<br>The code is found in the indexHTML() function.<br>You can add more sites if you need. Please see the well documented source code.<br><br>Use the following link to setup the network.<br><a href=\"setupNet.html\">NETWORK SETUP</a>";

P(Form_eth_start) = "<FORM action=\"setupNet.html\" method=\"get\">";
P(Form_end) = "<FORM>";
P(Form_input_send) = "<INPUT type=\"submit\" value=\"Set config\">";

P(Form_input_text_start) = "<input type=\"text\" name=\"";
P(Form_input_value)  = "\" value=\"";
P(Form_input_size2) = "\" maxlength=\"2\" size=\"2";
P(Form_input_size3) = "\" maxlength=\"3\" size=\"3";
P(Form_input_end) = "\">\n";

P(MAC) = "MAC address: ";
P(IP) = "IP address: ";
P(SUBNET) = "Subnet: ";
P(GW) = "GW address: ";
P(DNS_SERVER) = "DNS server: ";
P(WEB_PORT) = "Webserver port (1-65535): ";
P(DHCP_ACTIVE) = "Use DHCP: ";
P(DHCP_REFRESH) = "Renew interval for DHCP in minutes (1 - 255): ";

P(Form_cb) = "<input type=\"radio\" name=\"23\" value=\"";
P(Form_cb_checked) = " checked ";
P(Form_cb_on) = ">On";
P(Form_cb_off) = ">Off";

P(br) = "<br>\n";

P(table_start) = "<table>";
P(table_tr_start) = "<tr>";
P(table_tr_end) = "</tr>";
P(table_td_start) = "<td>";
P(table_td_end) = "</td>";
P(table_end) = "</table>";

P(Config_set) = "<font size=\"6\" color=\"red\">New configuration stored! <br>Please turn off and on your Arduino or use the reset button!</font><br>";

P(DHCP_STATE_TIME) = "DHCP last renew timestamp (sec)";
P(DHCP_STATE) = "DHCP renew return code (sec)";


P(UPTIME) = "Uptime: ";

#ifdef USE_SYSTEM_LIBRARY
P(RAM_1) = "RAM (byte): ";
P(RAM_2) = " free of ";
#endif

/* This creates an pointer to instance of the webserver. */
WebServer * webserver;


/**
* indexHTML() function
* This function is used to send the index.html to the client.
*/
void indexHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;
    
  server.printP(Page_start);
  
  server.printP(Index);
  
  server.printP(Page_end);
    
}

/**
* setupNetHTML() function
* This function is used to send the setupNet.html to the client.
*
* Overview:
* - Send a HTTP 200 OK Header
* - If get parameters exists assign them to the corresponding variable in the eeprom_config struct
* - Print the configuration
*
* Parameters are simple numbers. The name of the parameter is converted to an int with the atoi function.
* This saves some code for setting the MAC and IP addresses.
*/
#define NAMELEN 5
#define VALUELEN 7
void setupNetHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  URLPARAM_RESULT rc;
  char name[NAMELEN];
  char value[VALUELEN];
  boolean params_present = false;
  byte param_number = 0;

  /* this line sends the standard "we're all OK" headers back to the
     browser */
  server.httpSuccess();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;

  server.printP(Page_start);

  // check for parameters
  if (strlen(url_tail)) {
    while (strlen(url_tail)) {
      rc = server.nextURLparam(&url_tail, name, NAMELEN, value, VALUELEN);
      if (rc != URLPARAM_EOS) {
        params_present=true;
        // debug output for parameters
        #ifdef DEBUG
        Serial.print(name);
        server.print(name);
        Serial.print(" - "); 
        server.print(" - ");
        Serial.println(value);
        server.print(value);
        server.print("<br>");
        #endif
        
        
        param_number = atoi(name);
 
        // read MAC address
        if (param_number >=0 && param_number <=5) {
          eeprom_config.mac[param_number]=strtol(value,NULL,16);
        }
    
        // read IP address
        if (param_number >=6 && param_number <=9) {
          eeprom_config.ip[param_number-6]=atoi(value);
        }
    
        // read SUBNET
        if (param_number >=10 && param_number <=13) {
          eeprom_config.subnet[param_number-10]=atoi(value);
        }
    
        // read GATEWAY
        if (param_number >=14 && param_number <=17) {
          eeprom_config.gateway[param_number-14]=atoi(value);
        }
    
        // read DNS-SERVER
        if (param_number >=18 && param_number <=21) {
          eeprom_config.dns_server[param_number-18]=atoi(value);
        }
        
        // read WEBServer port
        if (param_number == 22) {
          eeprom_config.webserverPort=atoi(value);
        }
        
        // read DHCP ON/OFF
        if (param_number == 23) {
          eeprom_config.use_dhcp=atoi(value);
        }
    
        // read DHCP renew interval
        if (param_number == 24) {
          eeprom_config.dhcp_refresh_minutes=atoi(value);
        } 
      }
    }
    EEPROM_writeAnything(0, eeprom_config);
  }

  //print the form
  server.printP(Form_eth_start);
  
  if(params_present==true) {
     server.printP(Config_set);
  }
    
  server.printP(table_start);
  
  // print the current MAC
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(MAC);
  server.printP(table_td_end);
  server.printP(table_td_start);
  for (int a=0;a<6;a++) {
    server.printP(Form_input_text_start);
    server.print(a);
    server.printP(Form_input_value);
    server.print(eeprom_config.mac[a],HEX);
    server.printP(Form_input_size2);    
    server.printP(Form_input_end);
  }
  server.printP(table_td_end);
  server.printP(table_tr_end);

  // print the current IP
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(IP);
  server.printP(table_td_end);
  server.printP(table_td_start);    
  for (int a=0;a<4;a++) {
    server.printP(Form_input_text_start);
    server.print(a+6);
    server.printP(Form_input_value);
    server.print(eeprom_config.ip[a]);
    server.printP(Form_input_size3);
    server.printP(Form_input_end);
  }
  server.printP(table_td_end);
  server.printP(table_tr_end);
  

  // print the current SUBNET
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(SUBNET);
  server.printP(table_td_end);
  server.printP(table_td_start); 
  for (int a=0;a<4;a++) {
    server.printP(Form_input_text_start);
    server.print(a+10);
    server.printP(Form_input_value);
    server.print(eeprom_config.subnet[a]);
    server.printP(Form_input_size3);
    server.printP(Form_input_end);
  }
  server.printP(table_td_end);
  server.printP(table_tr_end);

  // print the current GATEWAY
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(GW);
  server.printP(table_td_end);
  server.printP(table_td_start); 
  for (int a=0;a<4;a++) {
    server.printP(Form_input_text_start);
    server.print(a+14);
    server.printP(Form_input_value);
    server.print(eeprom_config.gateway[a]);
    server.printP(Form_input_size3);
    server.printP(Form_input_end);
  }
  server.printP(table_td_end);
  server.printP(table_tr_end);

  // print the current DNS-SERVER
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(DNS_SERVER);
  server.printP(table_td_end);
  server.printP(table_td_start); 
  for (int a=0;a<4;a++) {
    server.printP(Form_input_text_start);
    server.print(a+18);
    server.printP(Form_input_value);
    server.print(eeprom_config.dns_server[a]);
    server.printP(Form_input_size3);
    server.printP(Form_input_end);
  }
  server.printP(table_td_end);
  server.printP(table_tr_end);

  
  // print the current webserver port
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(WEB_PORT);
  server.printP(table_td_end);
  server.printP(table_td_start);
  server.printP(Form_input_text_start);
  server.print(22);
  server.printP(Form_input_value);
  server.print(eeprom_config.webserverPort);
  server.printP(Form_input_end);
  server.printP(table_td_end);
  server.printP(table_tr_end);
  
  //print the current DHCP config
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(DHCP_ACTIVE);
  server.printP(table_td_end);
  server.printP(table_td_start);
  server.printP(Form_cb);
  server.print("0\"");
   if(eeprom_config.use_dhcp != 1) {
    server.printP(Form_cb_checked);
  }
  server.printP(Form_cb_off);   
  
  server.printP(Form_cb);
  server.print("1\"");
  if(eeprom_config.use_dhcp == 1) {
    server.printP(Form_cb_checked);
  }
  server.printP(Form_cb_on);   
  server.printP(table_td_end);
  server.printP(table_tr_end);
  
  //print the current DHCP renew time
  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(DHCP_REFRESH);
  server.printP(table_td_end);
  server.printP(table_td_start);
  server.printP(Form_input_text_start);
  server.print(24);
  server.printP(Form_input_value);
  server.print(eeprom_config.dhcp_refresh_minutes);
  server.printP(Form_input_size3);
  server.printP(Form_input_end);
  server.printP(table_td_end);
  server.printP(table_tr_end);

  //print DHCP status
  if(eeprom_config.use_dhcp == 1) {
    server.printP(table_tr_start);
    server.printP(table_td_start);	
    server.printP(DHCP_STATE);
    server.printP(table_td_end);
    server.printP(table_td_start);
    server.print(dhcp_state);
    server.printP(table_td_end);
    server.printP(table_tr_end);
	 
    server.printP(table_tr_start);
    server.printP(table_td_start);	
    server.printP(DHCP_STATE_TIME);
    server.printP(table_td_end);
    server.printP(table_td_start);
    server.print(last_dhcp_renew/1000);
    server.printP(table_td_end);
    server.printP(table_tr_end);
  }
  
  #ifdef USE_SYSTEM_LIBRARY
  //print uptime
  server.printP(table_tr_start);
  server.printP(table_td_start);	
  server.printP(UPTIME);
  server.printP(table_td_end);
  server.printP(table_td_start);
  server.print(sys.uptime());
  server.printP(table_td_end);
  server.printP(table_tr_end); 

  server.printP(table_tr_start);
  server.printP(table_td_start);
  server.printP(RAM_1);	
  server.print(sys.ramFree());
  server.printP(RAM_2);
  server.print(sys.ramSize());
  server.printP(table_td_end);
  server.printP(table_tr_end); 
  #endif
  
  server.printP(table_end);
  
  //print the send button
  server.printP(Form_input_send);    
  server.printP(Form_end);
  

  
  server.printP(Page_end);

}

/**
* errorHTML() function
* This function is called whenever a non extisting page is called.
* It sends a HTTP 400 Bad Request header and the same as text.
*/
void errorHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this line sends the standard "HTTP 400 Bad Request" headers back to the
     browser */
  server.httpFail();

  /* if we're handling a GET or POST, we can output our data here.
     For a HEAD request, we just stop after outputting headers. */
  if (type == WebServer::HEAD)
    return;
    
  server.printP(Http400);
  
  server.printP(Page_end);
}

// END WEBCODE ######################################################################################################################################################

/**
* setup() function
* This function is called whenever the arduino is turned on.
*/
void setup()
{
  Serial.begin(SERIAL_BAUD);
  
  /* initialize the Ethernet adapter with the settings from eeprom */
  delay(200); // some time to settle
  setupNetwork();
  delay(200); // some time to settle
  
  #define PREFIX ""
  webserver = new WebServer(PREFIX, eeprom_config.webserverPort);

  /* setup our default command that will be run when the user accesses
   * the root page on the server */
  webserver->setDefaultCommand(&indexHTML);

  /* setup our default command that will be run when the user accesses
   * a page NOT on the server */
  webserver->setFailureCommand(&errorHTML);

  /* run the same command if you try to load /index.html, a common
   * default page name */
  webserver->addCommand("index.html", &indexHTML);

  /* display a network setup form. The configuration is stored in eeprom */
  webserver->addCommand("setupNet.html", &setupNetHTML);

  /* start the webserver */
  webserver->begin();
}

/**
* loop() function
* Runs forver ....
* 
* Overview:
* - Renew the DHCP lease
* - Serve web clients
*
*/
void loop()
{
  // renew DHCP lease
  renewDHCP(eeprom_config.dhcp_refresh_minutes);

  char buff[200];
  int len = 200;

  /* process incoming connections one at a time forever */
  webserver->processConnection(buff, &len);
}
