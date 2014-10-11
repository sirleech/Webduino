#include "Arduino.h"
#include "System.h"

char* System::uptime()
{
  char buffer[65];

  long days=0;
  long hours=0;
  long mins=0;
  long secs=0;
  
  secs = millis()/1000; //convect milliseconds to seconds
  mins=secs/60; //convert seconds to minutes
  hours=mins/60; //convert minutes to hours
  days=hours/24; //convert hours to days
  secs=secs-(mins*60); //subtract the coverted seconds to minutes in order to display 59 secs max 
  mins=mins-(hours*60); //subtract the coverted minutes to hours in order to display 59 minutes max
  hours=hours-(days*24); //subtract the coverted hours to days in order to display 23 hours max

  if (days > 0) {
    ltoa(days,buffer,10);
    strcpy(retval,buffer);
  } 
  else {
    strcpy(retval,"0");
  }

  strcat(retval,":");
  ltoa(hours,buffer,10);
  strcat(retval,buffer);

  strcat(retval,":");
  ltoa(mins,buffer,10);
  strcat(retval,buffer);

  strcat(retval,":");
  ltoa(secs,buffer,10);
  strcat(retval,buffer);

  strcat(retval,'\0');

  return retval;
}

int System::ramFree () {
  extern int __heap_start, *__brkval; 
  int v;
  int a = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
  return a;
}

int System::ramSize() {
  int v;
  int a = (int) &v;  
  return a;
}


