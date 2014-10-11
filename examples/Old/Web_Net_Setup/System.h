#ifndef System_h
#define System_h

#include <Arduino.h>

/**
* System Class.
*
* @author Matthias Maderer
* @version 1.1.7
*/
class System {
public:
  /**
  * Returns the uptime of the arduino with a char pointer.
  * Format: DAYS:HOURS:MINUTES:SECONDS
  * Sample: 1:20:23:50 = 1 day, 20 hours, 23 minutes and 50 seconds
  * @return char *: pointer!
  */
  char * uptime();
  
  /**
  * Returns the free RAM
  * @return int: free RAM
  */ 
  int ramFree();
  
  /**
  * Returns the size of the RAM
  * @return int: RAM size
  */ 
  int ramSize();
  
private:
  char retval[25];
};

#endif

