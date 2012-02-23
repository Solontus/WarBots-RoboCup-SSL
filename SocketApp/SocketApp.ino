/*
 * Socket App
 *
 * A simple socket application example using the WiShield 1.0
 */

#include <WiShield.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

const int wheelFL = 2;
const int wheelFR = 4;
const int wheelBK = 7;
const int PWMFL = 3;
const int PWMFR = 5;
const int PWMBK = 6;

class instruction{
  public:
    bool compound;
    double FL1;
    double FR1;
    double BK1;
    double FL2;
    double FR2;
    double BK2;
    
    instruction(double a1, double a2, double a3){
       compound = false;
       FL1 = a1;
       FR1 = a2;
       BK1 = a3;
    }
    
    instruction(double a11, double a21, double a31, double a12, double a22, double a32){
       compound = true;
       FL1 = a11;
       FR1 = a21;
       BK1 = a31;
       FL2 = a12;
       FR2 = a22;
       BK2 = a32;
    }
    
    instruction(){
    }
};

instruction INSTR;
bool useInstr = false;

char *cmd_message = "0 0                                            \0";

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,1,147};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"WarBots RoboCup"};		// max 32 bytes

unsigned char security_type = 0;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"password"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 1
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 2
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00	// Key 3
								};

void stopMovement(){
    analogWrite(PWMFL, 0);
    analogWrite(PWMFR, 0);
    analogWrite(PWMBK, 0);
    digitalWrite(wheelFL, LOW);
    digitalWrite(wheelFR, LOW);
    digitalWrite(wheelBK, LOW);
}

void setWheelFL(double spd){
  int power = abs((int)(spd * 255.0));
  if(spd > 0){
    analogWrite(PWMFL, power);
    digitalWrite(wheelFL, HIGH);;      
  }
  else if(spd < 0){
    analogWrite(PWMFL, power);
    digitalWrite(wheelFL, LOW);  
  }
  else{
     //Serial.println("FL stop");
     analogWrite(PWMFL, 0);
     digitalWrite(wheelFL, LOW);
  }
}

void setWheelFR(double spd){
  int power = abs((int)(spd * 255.0));
  if(spd > 0){
    analogWrite(PWMFR, power);
    digitalWrite(wheelFR, HIGH);   
  }
  else if(spd < 0){
    analogWrite(PWMFR, power);
    digitalWrite(wheelFR, LOW);  
  }
  else{
     //Serial.println("FR stop");
     analogWrite(PWMFR, 0);
     digitalWrite(wheelFR, LOW);
  }
}

void setWheelBK(double spd){
  int power = abs((int)(spd * 255.0));
  if(spd > 0){
    analogWrite(PWMBK, power);
    digitalWrite(wheelBK, HIGH);    
  }
  else if(spd < 0){
    analogWrite(PWMBK, power);
    digitalWrite(wheelBK, LOW); 
  }
  else{
     //Serial.println("BK stop");
     analogWrite(PWMBK, 0);
     digitalWrite(wheelBK, LOW);
  }
}

void rotate(double spd){
      setWheelFL(spd);
      setWheelFR(spd);
      setWheelBK(spd);
}


void runInstr(){
    if(!INSTR.compound){
        setWheelFL(INSTR.FL1);
        setWheelFR(INSTR.FR1);
        setWheelBK(INSTR.BK1);
    }
    else{
        setWheelFL(INSTR.FL1);
        setWheelFR(INSTR.FR1);
        setWheelBK(INSTR.BK1);
        delay(500);
        setWheelFL(INSTR.FL2);
        setWheelFR(INSTR.FR2);
        setWheelBK(INSTR.BK2);
        delay(500);
        stopMovement();
    }
}

void process_message(){
               
        if(strlen(cmd_message) < 2){
              return;
        }
        
    	char * pch;
    	int command = 0;
    	double arguments[4] = {0.0};
    
        pch = strtok(cmd_message," ,\n");
    
    	//command type
    	command = atoi(pch);
    
    	pch = strtok (NULL, " ,\n");
    	int counter = 0;
        while (pch != NULL)
        {
    		arguments[counter] = atof(pch);		
    		printf("%f\n", arguments[counter]);
    		counter++;
            pch = strtok (NULL, " ,\n");
    
        }
    
        useInstr = false;
        
    	//execute
    	switch(command){
                
    		case 0:
    			//stop
    			//arguments = {roller, 0, 0, 0}
    			Serial.print("stop, with roller speed = ");
                        Serial.println(arguments[0]);
                        stopMovement();
    			break;
    		case 1:
    			//move: send motor speeds
    			//arguments = {v, v_dir (radians), roller, 0}
    			Serial.print("drive: v = ");
    			Serial.print(arguments[0]);
                        Serial.print(" v_dir = ");
                        Serial.print(arguments[1]);
                        Serial.print(" roller speed = ");
                        Serial.println(arguments[2]);

                        //div up the work
                        //axis one(60 deg): +ve 0 -ve
                        //axis two(180 deg): -ve +ve 0
                        //axis three(300 deg): 0 -ve +ve
                        
                        /*
                        * dir = dir % 2pi
                        * if dir < 2pi / 3 or > 5pi / 3
                        *    axis 1 + 2
                        * else if dir < pi
                        *    axis 2 + 3
                        * else if dir < 5pi / 3
                        *    axis 3 + 1
                        *
                        * v1 = v cos(dir)
                        * v2 = v*cos(pi - dir)
                        */
                        
                        double v = arguments[0];
                        double dir = arguments[1] % (2*PI);
                        double v1 = v * cos(dir);
                        double v2 = v * cos(PI - dir);
                        
                        if((dir <= 2*PI/3) || (dir > 5*PI/3)){
                            INSTR = instruction(v1, 0, -v1, -v2, v2, 0);
                        }
                        else if(dir <= PI){
                            INSTR = instruction(-v1, v1, 0, 0, -v2, v2);
                        }
                        else{ //dir <= 5*PI/3
                            INSTR = instruction(0, -v1, v1, v2, 0, -v2);
                        }
                            
                        useInstr = true;

    			break;
    		case 2:
    			//rotate: +ve is counterclockwise
    			//arguments = {v_ROT, roller, 0, 0}    
                        Serial.print("rotate: v_ROT = ");
                        Serial.print(arguments[0]);
                        Serial.print(" roller speed = ");
                        Serial.println(arguments[1]);
                        
                        rotate(arguments[0]);
    			break;
    		case 3:
    			//kick
    			//arguments = {roller, 0, 0, 0}
    			Serial.print("kick, with roller speed = ");
                        Serial.println(arguments[3]);
                        //stopMovement();
    			break;
    	}
}



// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;
//---------------------------------------------------------------------------

void setup()
{
        Serial.begin(9600);
	WiFi.init();
        Serial.print("---Program");
        Serial.println(" Begins---");
        pinMode(wheelFL, OUTPUT);
        pinMode(wheelFR, OUTPUT);
        pinMode(wheelBK, OUTPUT);
        pinMode(PWMFL, OUTPUT);
        pinMode(PWMFR, OUTPUT);
        pinMode(PWMBK, OUTPUT);  
}

void loop()
{
        process_message();
        
        if(useInstr)
            runInstr();

	WiFi.run();
        delay(10);
}
