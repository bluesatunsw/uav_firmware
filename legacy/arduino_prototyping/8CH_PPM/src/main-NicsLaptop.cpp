// https://playground.arduino.cc/Interfacing/CPPWindows/

////////////////////////////////////////
// Reads PPM Serial Output /////////////
// For Prototyping (Alt to py script) //
////////////////////////////////////////

#include <stdio.h>
#include <tchar.h>
#include "SerialClass.h"	
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct record {
	const char *channel;
	int value;
};


// vector<string> channels = {"Yaw","Pitch","Throt","Roll","2Way","3Way1","3Way2","Pot"};
const char* channels[ARRAY_LENGTH] = {"Yaw","Pitch","Throt","Roll","2Way","3Way1","3Way2","Pot"};

// char output[ARRAY_LENGTH];
int incomingData[ARRAY_LENGTH];

// change the name of the port with the port name of your computer
// must remember that the backslashes are essential so do not remove them
// char *port = "\\\\.\\COM5";

// int main(){
// 	Serial arduino(port);
// 	if(arduino.IsConnected()){
// 		cout<<"Connection made"<<endl<<endl;
// 	}
// 	else{
// 		cout<<"Error in port name"<<endl<<endl;
// 	}
// 	while(arduino.IsConnected()){
// 		cout<<"Enter your command: "<<endl;
// 		string data;
// 		cin>>data;

// 		char *charArray = new char[data.size() + 1];
// 		copy(data.begin(), data.end(), charArray);
// 		charArray[data.size()] = '\n';

// 		arduino.WriteData(charArray, MAX_DATA_LENGTH);
// 		arduino.ReadData(output, MAX_DATA_LENGTH);

// 		cout<<">> "<<output<<endl;

// 		delete [] charArray;
// 	}
// 	return 0;
// }

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{
	printf("Welcome to the serial test!\n\n");

	Serial* SP = new Serial("\\\\.\\COM5");   

	if (SP->IsConnected())
		printf("We're connected");

	char incomingData[256] = "";			
	//printf("%s\n",incomingData);
	int dataLength = 255;
	int readResult = 0;
	struct record curPair[ARRAY_LENGTH];
	struct record prevPair[ARRAY_LENGTH];

	while(SP->IsConnected())
	{

		readResult = SP->ReadData(incomingData, ARRAY_LENGTH);
		printf("Bytes read: (0 means no data available) %i\n",readResult);
		incomingData[readResult] = 0;

		printf("%s",incomingData);

		// VectorPair(curPair)
		for (int i = 0; i < 8; i++) {
			curPair[i].channel = channels[i];
			curPair[i].value = incomingData[i];
			
		}

		if (prevPair == NULL) {
			for (int i = 0; i < ARRAY_LENGTH; i++) {
				prevPair[i] = curPair[i];
			}
			continue;
		}
		for (int i = 0; i < ARRAY_LENGTH; i++) {
			int diff = abs(curPair->value - prevPair->value);
			if (diff > CH_DELTA) {
				cout << "Change in " << curPair[i].channel << " from " << prevPair[i].value << " to " << curPair[i].value;
			}
		}
		for (int i = 0; i < ARRAY_LENGTH; i++) {
			prevPair[i] = curPair[i];
		}
		Sleep(500);
	
	}
	return 0;
}

    // vector<pair<string, int>> myVector;
    
    // const char *str[8] = {"Yaw","Pitch","Throt","Roll","2Way","3Way1","3Way2","Pot"};
    // int arr[8] = {1,2,3,4,5,6,7,8};
    // for (int i = 0; i < 8; i++) {
    //     // map[str[i]] = arr[i];
    //     myVector.push_back(make_pair(str[i], arr[i]));
    // }
    // for(int i =0;i<myVector.size();i++)
    // {
    //     cout<<"("<<myVector[i].first<<","<<myVector[i].second<<")"<<" ";
    // }