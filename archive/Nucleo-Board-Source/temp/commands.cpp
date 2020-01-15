//MODIFIED FROM 

//------------------------------------------------------------------------------
// 4 Axis CNC Demo  - supports MEGA 2560 Arduino RAMPS 1.4
// dan@marginallyclever.com 2013-10-28
// Modified by Søren Vedel
// sorenvedel@gmail.com 2015-06-19
// Modified by Michael Lenover
// mlenover@gmail.com 2019-12-01
//------------------------------------------------------------------------------
// Copyright at end of file.
// please see http://www.github.com/MarginallyClever/GcodeCNCDemo for more information.


//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------
//#define VERBOSE              (1)  // add to get a lot more serial output.

#include "commands.h"

//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------
Motor motors[NUM_AXIES];

char serialBuffer[MAX_BUF];  // where we store the message until we get a ';'
int sofar;  // how much is in the buffer

// speeds
float fr=0;       // FEEDRATE
long step_delay;  // delay between steps

float px,pz,pa;  // position

// settings
char mode_abs=1;  // absolute mode?

long line_number=0;


//------------------------------------------------------------------------------
// METHODS
//------------------------------------------------------------------------------


/**
 * delay for the appropriate number of microseconds
 * @input ms how many milliseconds to wait
 */
void pause(long ms) {
  //delay(ms/1000);
  delayMicroseconds(ms%1000);  // delayMicroseconds doesn't work for values > ~16k.
}


/**
 * Set the feedrate (speed motors will move)
 * @input new feedrate (nfr) the new speed in steps/second
 */
void feedrate(float nfr) {
  if(fr==nfr) return;  // same as last time?  quit now.

  if(nfr>MAX_FEEDRATE || nfr<MIN_FEEDRATE) {  // don't allow crazy feed rates
    Serial.print(F("New feedrate must be greater than "));
    Serial.print(MIN_FEEDRATE);
    Serial.print(F("steps/s and less than "));
    Serial.print(MAX_FEEDRATE);
    Serial.println(F("steps/s."));
    return;
  }
  step_delay = MAX_FEEDRATE/nfr;
  fr=nfr;
}


/**
 * Set the logical position
 * @input npx new position x
 * @input npy new position y
 */
void position(float npx,float npz,float npa) {
  // here is a good place to add sanity tests
  px=npx;
  pz=npz;
  pa=npa;
}


/**
 * Supports movement with both styles of Motor Shield
 * @input newx the destination x position
 * @input newy the destination y position
 **/
void onestep(int motor) {
#ifdef VERBOSE
  char *letter="XZA";
  Serial.print(letter[]);
#endif
  
  digitalWrite(motors[motor].step_pin,HIGH);
  digitalWrite(motors[motor].step_pin,LOW);
}

/**
 * Uses bresenham's line algorithm to move both motors
 * @input newx the destination x position
 * @input newy the destination y position
 **/
void line(float newx,float newz,float newa) {
  //SET DIRECTION OF MOTORS
  a[0].delta = (newx-px)*STEPS_PER_MM;
  a[1].delta = (newz-pz)*STEPS_PER_MM;
  a[2].delta = (newa-pa)*STEPS_PER_MM;
  
  long i,j,maxsteps=0;

  for(i=0;i<NUM_AXIES;++i) {
    a[i].absdelta = abs(a[i].delta);
    if( maxsteps < a[i].absdelta ) maxsteps = a[i].absdelta;
    // set the direction once per movement
    digitalWrite(motors[i].dir_pin,a[i].delta>0?HIGH:LOW);
  }
  for(i=0;i<NUM_AXIES;++i) {
    a[i].over= maxsteps/2;
  }
  
  long dt = MAX_FEEDRATE/5000;
  long accel = 1;
  long steps_to_accel = dt - step_delay;
  if(steps_to_accel > maxsteps/2 ) 
    steps_to_accel = maxsteps/2;
    
  long steps_to_decel = maxsteps - steps_to_accel;

  //PRINT SOME STUFF
  Serial.print("START ");
  Serial.println(dt);
  Serial.print("TOP ");
  Serial.println(step_delay);
  
  Serial.print("accel until ");
  Serial.println(steps_to_accel);  
  Serial.print("decel after ");
  Serial.println(steps_to_decel);  
  Serial.print("total ");
  Serial.println(maxsteps);  
#ifdef VERBOSE
  Serial.println(F("Start >"));
#endif

  for( i=0; i<maxsteps; ++i ) {
    for(j=0;j<NUM_AXIES;++j) {
      a[j].over += a[j].absdelta;
      if(a[j].over >= maxsteps) {
        a[j].over -= maxsteps;
        
        digitalWrite(motors[j].step_pin,HIGH);
        digitalWrite(motors[j].step_pin,LOW);
      }
    }

    if(i<steps_to_accel) {
      dt -= accel;
    }
    if(i>=steps_to_decel) {
      dt += accel;
    }
    delayMicroseconds(dt);
  }

#ifdef VERBOSE
  Serial.println(F("< Done."));
#endif

  position(newx,newz,newa);

  where();
}

/**
 * Look for character /code/ in the serialBuffer and read the float that immediately follows it.
 * @return the value found.  If nothing is found, /val/ is returned.
 * @input code the character to look for.
 * @input val the return value if /code/ is not found.
 **/
float parseNumber(char code,float val) {
  char *ptr=serialBuffer;  // start at the beginning of serialBuffer
  while((long)ptr > 1 && (*ptr) && (long)ptr < (long)serialBuffer+sofar) {  // walk to the end
    if(*ptr==code) {  // if you find code on your walk,
      return atof(ptr+1);  // convert the digits that follow into a float and return it
    }
    ptr=strchr(ptr,' ')+1;  // take a step from here to the letter after the next space
  }
  return val;  // end reached, nothing found, return default val.
}



/**
 * write a string followed by a float to the serial line.  Convenient for debugging.
 * @input code the string.
 * @input val the float.
 */
void output(const char *code,float val) {
  Serial.print(code);
  Serial.println(val);
}


/**
 * print the current position, feedrate, and absolute mode.
 */
void where() {
  output("X",px);
  output("Z",pz);
  output("A",pa);
  output("F",fr);
  Serial.println(mode_abs?"ABS":"REL");
} 


/**
 * display helpful information
 */
void help() {
  Serial.print(F("GcodeTubeNotcherV1"));
  Serial.println(VERSION);
  Serial.println(F("Commands:"));
  Serial.println(F("G00/G01 [X/Z/A(steps)] [F(feedrate)]; - linear move"));
  Serial.println(F("G04 P[seconds]; - delay"));
  Serial.println(F("G90; - absolute mode"));
  Serial.println(F("G91; - relative mode"));
  Serial.println(F("G92 [X/Z/E(steps)]; - change logical position"));
  Serial.println(F("M18; - disable motors"));
  Serial.println(F("M100; - this help message"));
  Serial.println(F("M114; - report position and feedrate"));
  Serial.println(F("All commands must end with a newline."));
}


/**
 * Read the serialBuffer and find any recognized commands.  One G or M command per line.
 */
void processCommand() {
  int cmd = parseNumber('G',-1);
  switch(cmd) {
  case  0:
  case  1: { // line
    feedrate(parseNumber('F',fr));
    line( parseNumber('X',(mode_abs?px:0)) + (mode_abs?0:px),
          parseNumber('Z',(mode_abs?pz:0)) + (mode_abs?0:pz),
          parseNumber('A',(mode_abs?pe:0)) + (mode_abs?0:pe) );
    break;
    }
  case  2:
  case  4:  pause(parseNumber('P',0)*1000);  break;  // dwell
  case 90:  mode_abs=1;  break;  // absolute mode
  case 91:  mode_abs=0;  break;  // relative mode
  case 92:  // set logical position
    position( parseNumber('X',0),
              parseNumber('Z',0),
              parseNumber('E',0) );
    break;
  default:  break;
  }

  cmd = parseNumber('M',-1);
  switch(cmd) {
  case  17:  motor_enable();  break;
  case  18:  motor_disable();  break;
  case 100:  help();  break;
  case 114:  where();  break;
  default:  break;
  }
}


/**
 * prepares the serialBuffer to receive a new message and tells the serial connected device it is ready for more.
 */
void ready() {
  sofar=0;  // clear serialBuffer
  Serial.print(F(">"));  // signal ready to receive input
}


/**
 * set up the pins for each motor
 * Pins fits a Ramps 1.4 board
 */
void motor_setup() {
  motors[0].step_pin=4;
  motors[0].dir_pin=34;
  motors[0].enable_pin=35;
  motors[0].limit_switch_pin=36;

  motors[1].step_pin=60;
  motors[1].dir_pin=61;
  motors[1].enable_pin=56;
  motors[1].limit_switch_pin=14;

  motors[2].step_pin=46;
  motors[2].dir_pin=48;
  motors[2].enable_pin=62;
  motors[2].limit_switch_pin=18;

  motors[3].step_pin=26;
  motors[3].dir_pin=28;
  motors[3].enable_pin=24;
  motors[3].limit_switch_pin=34;
  
  int i;
  for(i=0;i<NUM_AXIES;++i) {  
    // set the motor pin & scale
    pinMode(motors[i].step_pin,OUTPUT);
    pinMode(motors[i].dir_pin,OUTPUT);
    pinMode(motors[i].enable_pin,OUTPUT);
  }
}


void motor_enable() {
  int i;
  for(i=0;i<NUM_AXIES;++i) {  
    digitalWrite(motors[i].enable_pin,LOW);
  }
}


void motor_disable() {
  int i;
  for(i=0;i<NUM_AXIES;++i) {  
    digitalWrite(motors[i].enable_pin,HIGH);
  }
}


/**
 * First thing this machine does on startup.  Runs only once.
 */
void setup() {
  Serial.begin(BAUD);  // open coms

  motor_setup();
  motor_enable();
  
  
  where();  // for debugging purposes
  help();  // say hello
  position(0,0,0,0);  // set starting position
  feedrate(1000);  // set default speed
  ready();
}


/**
 * After setup() this machine will repeat loop() forever.
 */
void loop() {
  // listen for serial commands
  while(Serial.available() > 0) {  // if something is available
    char c=Serial.read();  // get it
    Serial.print(c);  // repeat it back so I know you got the message
    if(sofar<MAX_BUF-1) serialBuffer[sofar++]=c;  // store it
    if(c=='\n') {
      // entire message received
      serialBuffer[sofar]=0;  // end the buffer so string functions work right
      Serial.print(F("\r\n"));  // echo a return character for humans
      processCommand();  // do something with the command
      ready();
    }
  }
}


/**
* This file is part of GcodeCNCDemo.
*
* GcodeCNCDemo is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* GcodeCNCDemo is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Foobar. If not, see <http://www.gnu.org/licenses/>.
*/
