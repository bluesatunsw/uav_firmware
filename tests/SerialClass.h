#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#define ARDUINO_WAIT_TIME 2000

#define YAW 0
#define PITCH 1
#define THROTTLE 2
#define ROLL 3
#define B2WAY 4
#define B3WAY1 5
#define B3WAY2 6
#define POT 7
#define ARRAY_LENGTH 8
#define MAX_DATA_LENGTH 8 
#define CH_DELTA 4

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

class Serial
{
    private:
        //Serial comm handler
        HANDLE hSerial;
        //Connection status
        bool connected;
        //Get various information about the connection
        COMSTAT status;
        //Keep track of last error
        DWORD errors;

    public:
        //Initialize Serial communication with the given COM port
        Serial(const char *portName);
        //Close the connection
        ~Serial();
        //Read data in a buffer, if nbChar is greater than the
        //maximum number of bytes available, it will return only the
        //bytes available. The function return -1 when nothing could
        //be read, the number of bytes actually read.
        int ReadData(char *buffer, unsigned int nbChar);
        //Writes data from a buffer through the Serial connection
        //return true on success.
        bool WriteData(const char *buffer, unsigned int nbChar);
        //Check if we are actually connected
        bool IsConnected();
        // void VectorPair(const char *str, int arr, std::vector<std::pair<std::string, int>> myVector);


};

#endif // SERIALCLASS_H_INCLUDED