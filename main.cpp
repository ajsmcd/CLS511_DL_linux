/**
 * @file       main.cpp
 * @author     Aaron McDonald
 * @license    This project is released under the MIT License (MIT)
 * @date       August 2017
 * @Notes      Modification of original blynk code by Volodymyr Shymanskyy
 */

//myCLS Functions and Blynk Libraries
#include "myCLS.h"

//Blynk Widget Objects
#define xv5111_HS_Widget V0	//Hand-Switch button widget for xv5111
#define interval_Widget V1	//Interval selector widget
#define sched_Widget V2		//Schedule selector widget
#define x1_Mode_Widget V3	//Mode selector widget for Loop x1

//Control Variables
const int xv5111_DO=17;		//Digital out pin for xv5111
bool xv5111_DO_State;		//xv5111 digital out current state
bool xv5111_HS_DI;		//Soft digital input from xv5111_HS_Widget
int interval_UXI;	        //Input watering time interval
int sched_Start_DAY;		//Calculated scheduled start day
int sched_Start_HR;		//Input watering schedule - start hr
int sched_Start_MIN;		//Input watering schedule - start min
bool sched_MON;			//Input watering schedule - on Monday?
bool sched_TUE;			//Input watering schedule - on Tuesday?
bool sched_WED;			//Input watering schedule - on Wednesday?
bool sched_THU;			//Input watering schedule - on Thursday?
bool sched_FRI;			//Input watering schedule - on Friday?
bool sched_SAT;			//Input watering schedule - on Saturday?
bool sched_SUN;			//Input watering schedule - on Sunday?
int current_DAY;		//Current day
int current_HR;			//Current hour
int current_MIN;		//Current minute
int sched_Stop_DAY;		//Watering time stop - day
int sched_Stop_HR;		//Watering time stop - hour
int sched_Stop_MIN;		//Watering time stop - minute
Mode x1_Mode_XI;		//Current Loop x1 mode
bool next_Date_Required;	//Recalculate the next scheduled start and stop
bool sched_NULL;		//Null Schedule Set
bool sched_Triggered;		//Schedule is Currently Active

enum weekDAY{mon=1,tue=2,wed=3,thu=4,fri=5,sat=6,sun=7}; // weekDAY enum list

bool dayIsScheduled(int input_DAY);
int myProcess();
bool schedNOW(int input_DAY,int input_HR,int input_MIN);

//Operator Soft Hand Switch Input Function
BLYNK_WRITE(xv5111_HS_Widget){
 xv5111_HS_DI = param[0].asInt();

 x1_Mode_XI = Manual;
 Blynk.virtualWrite(x1_Mode_Widget,x1_Mode_XI);
 next_Date_Required = true;

 xv5111_DO_State = xv5111_HS_DI;
 digitalWrite(xv5111_DO,xv5111_DO_State);

 cout<<"Operator soft HS pressed"<<endl;
 cout<<" > xv5111_HS_DI: "<<xv5111_HS_DI<<endl;
 cout<<" > x1_Mode_XI: "<<modetoString(x1_Mode_XI)<<endl;}

//New Interval Set Function
BLYNK_WRITE(interval_Widget){
 interval_UXI = param[0].asInt();
 next_Date_Required = true;

 cout<<"Watering time interval modified: "<<interval_UXI<<" minutes"<<endl;}

//New Schedule Set Function
BLYNK_WRITE(sched_Widget){
 TimeInputParam t(param);

 sched_Start_HR = t.getStartHour();
 sched_Start_MIN = t.getStartMinute();
 sched_MON = t.isWeekdaySelected(1);
 sched_TUE = t.isWeekdaySelected(2);
 sched_WED = t.isWeekdaySelected(3);
 sched_THU = t.isWeekdaySelected(4);
 sched_FRI = t.isWeekdaySelected(5);
 sched_SAT = t.isWeekdaySelected(6);
 sched_SUN = t.isWeekdaySelected(7);

 next_Date_Required = true;

 cout<<"Watering schedule updated: "<<endl;
 cout<<" > Start Hour: "<<sched_Start_HR<<endl;
 cout<<" > Start Minute: "<<sched_Start_MIN<<endl;
 cout<<" > Monday: "<<sched_MON<<endl;
 cout<<" > Tuesday: "<<sched_TUE<<endl;
 cout<<" > Wednesday "<<sched_WED<<endl;
 cout<<" > Thursday: "<<sched_THU<<endl;
 cout<<" > Friday: "<<sched_FRI<<endl;
 cout<<" > Saturday: "<<sched_SAT<<endl;
 cout<<" > Sunday: "<<sched_SUN<<endl;}

//xv5111 Mode Set Function
BLYNK_WRITE(x1_Mode_Widget){
 x1_Mode_XI = inttoMode(param[0].asInt());

 if (x1_Mode_XI == Auto){
  next_Date_Required = true;}

 cout<<"Operating mode modified: "<<endl;
 cout<<" > Mode: "<<modetoString(x1_Mode_XI)<<endl;}

//Current System Time Function
int getSystemTime(){
 time_t now;
 struct tm nowlocal;
 now = time(NULL);
 nowlocal =*localtime(&now);

 current_DAY = nowlocal.tm_wday;

 if (current_DAY == 0){
  current_DAY = 7;}

 current_HR = nowlocal.tm_hour;
 current_MIN = nowlocal.tm_min;
 return 0;}

//Current Day In Schedule
int getNextDueDate(){
 int temp_DAY;
 bool temp_DAY_Scheduled;
 int temp_MIN;
 int temp_HR;
 bool start_Defined;

 start_Defined = false;
 sched_NULL = false;
 sched_Triggered = false;

 for (int i=0; i<7; i++){
  temp_DAY = current_DAY + i;

  if (temp_DAY > 7){
  temp_DAY = temp_DAY - 7;}

  temp_DAY_Scheduled = dayIsScheduled(temp_DAY);

  if (temp_DAY_Scheduled && temp_DAY == current_DAY){
   if (current_HR < sched_Start_HR){
    sched_Start_DAY = temp_DAY;
    start_Defined = true;
    break;}
   else if (current_HR == sched_Start_HR && current_MIN <= sched_Start_MIN){
    sched_Start_DAY = temp_DAY;
    start_Defined = true;
    break;}
  }
  else if (temp_DAY_Scheduled && temp_DAY != current_DAY){
    sched_Start_DAY = temp_DAY;
    start_Defined = true;
    break;
  }}

 if (start_Defined == false){
  sched_NULL = true;
  next_Date_Required = false;
  return 0;}

 temp_MIN = interval_UXI + sched_Start_MIN;

 if (temp_MIN <= 59){
  sched_Stop_MIN = temp_MIN;
  sched_Stop_HR = sched_Start_HR;
  sched_Stop_DAY = sched_Start_DAY;

  next_Date_Required = false;
  return 0;}
 else{
  sched_Stop_MIN = temp_MIN - 60;
  temp_HR = sched_Start_HR + 1;

  if (temp_HR == 24){
   sched_Stop_HR = 1;
   sched_Stop_DAY = sched_Start_DAY + 1;

   next_Date_Required = false;
   return 0;}
  else{
   sched_Stop_HR = temp_HR;
   sched_Stop_DAY = sched_Start_DAY;

   next_Date_Required = false;
   return 0;}}}

//Validate Day is Scheduled Function
bool dayIsScheduled (int input_DAY){
   if (input_DAY == 1 && sched_MON == true){
    return true;}
   else if (input_DAY == 2 && sched_TUE == true){
    return true;}
   else if (input_DAY == 3 && sched_WED == true){
    return true;}
   else if (input_DAY == 4 && sched_THU == true){
    return true;}
   else if (input_DAY == 5 && sched_FRI == true){
    return true;}
   else if (input_DAY == 6 && sched_SAT == true){
    return true;}
   else if (input_DAY == 7 && sched_SUN == true){
    return true;}
   else {return false;}}

//Check Schedule Matches NOW Function
bool schedNOW(int input_DAY,int input_HR,int input_MIN){
 if (current_DAY == input_DAY && current_HR == input_HR && current_MIN == input_MIN){
  return true;}
 else{
  return false;}}

//Primary Process Function - Called from Main()
int myProcess(){

 //Get current system time
 getSystemTime();

 //Determine if system is in Auto
 if (x1_Mode_XI != Auto){
  return 0;}

 //Get next scheduled start and stop times
 if (next_Date_Required == true){
  getNextDueDate();

  cout<<"Next Due Date Calculated: "<<endl;
  if (sched_NULL == true){
   cout<<" > Null Schedule"<<endl;
   return 0;}
  else{
   cout<<" > Start Day: "<<sched_Start_DAY<<endl;
   cout<<" > Start Hour: "<<sched_Start_HR<<endl;
   cout<<" > Start Minute: "<<sched_Start_MIN<<endl;
   cout<<" > Stop DAY: "<<sched_Stop_DAY<<endl;
   cout<<" > Stop Hour: "<<sched_Stop_HR<<endl;
   cout<<" > Stop Minute: "<<sched_Stop_MIN<<endl;
   cout<<endl;}}

 //Check whether or not to start or start
 bool temp_Process_Check_Start;
 bool temp_Process_Check_Stop;

 temp_Process_Check_Start = schedNOW(sched_Start_DAY,sched_Start_HR,sched_Start_MIN);
 temp_Process_Check_Stop = schedNOW(sched_Stop_DAY,sched_Stop_HR,sched_Stop_MIN);

 if (temp_Process_Check_Start == true && sched_Triggered == false){
   digitalWrite(xv5111_DO,relay_ON);
   xv5111_DO_State = 1;
   Blynk.virtualWrite(xv5111_HS_Widget,xv5111_DO_State);
   sched_Triggered = true;}
 else if (temp_Process_Check_Stop == true && sched_Triggered == true){
   digitalWrite(xv5111_DO,relay_OFF);
   xv5111_DO_State = 0;
   Blynk.virtualWrite(xv5111_HS_Widget,xv5111_DO_State);
   next_Date_Required = true;
   sched_Triggered = false;}

 return 0;}

//Initialisation Function
void setup(){

 //Pin Definitions
 pinMode(xv5111_DO,OUTPUT);

 //Default Values
 digitalWrite(xv5111_DO,relay_OFF);
 x1_Mode_XI = Auto;
 sched_Start_DAY = 7;
 sched_Start_HR = 10;
 sched_Start_MIN = 0;
 sched_MON = 1;
 sched_TUE = 1;
 sched_WED = 1;
 sched_THU = 1;
 sched_FRI = 1;
 sched_SAT = 1;
 sched_SUN = 1;
 interval_UXI = 1;

 next_Date_Required = true;

 cout<<"Default Settings: "<<endl;
 cout<<" > XV5111 relay Off"<<endl;
 cout<<" > Mode: Auto"<<endl;
 cout<<" > Schedule: Everyday @ 10:00 am"<<endl;
 cout<<endl;}

//Main() Execution Function
int main(int argc, char* argv[]){
 const char *auth, *serv;
 uint16_t port;
 parse_options(argc, argv, auth, serv, port);

 Blynk.begin(auth, serv, port);

 setup();

 while(true){
  //Run Blynk processes
  Blynk.run();

  //Get cumulative System operating time in seconds
  uptime = (millis() / 1000);

  //Run custom processes on 1 second interval
  if(millis() >= uptime + 1){
   myProcess();}}

  return 0;}
