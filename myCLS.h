//myCLS.h

//Libraries
#define BLYNK_PRINT stdout
#include <BlynkApiWiringPi.h>
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

#include <BlynkWidgets.h>
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

//Enum Types
enum Mode {Auto=0,Manual=1};	//Mode value list

//Relay State Handling
enum relState {relay_OFF=0,relay_ON=1};

//Integer to Mode Conversion Function
Mode inttoMode(int inputInt)
{
 if (inputInt == 0)
 {
  return Auto;
 }
 else
 {
  return Manual;
 }
}

//Mode to String Conversion Function
string modetoString (Mode inputMode)
{
 if (inputMode == Auto)
 {
  return "Auto";
 }
 else
 {
  return "Manual";
 }
}

//Custom Timer Function
unsigned int uptime;

