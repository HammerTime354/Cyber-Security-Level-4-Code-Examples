//Communication with the gyro
#include <Wire.h>                          
//Enables storage of information onto the EEPROM (Electronically Erasable Programmable Read-Only Memory)
#include <EEPROM.h>                        


//PID settings
//Maximum output of the roll PID-controller (+/-)
float pid_p_gain_roll = 1.3;               
float pid_i_gain_roll = 0.04;              
float pid_d_gain_roll = 18.0;              
//Maximum output of the PID-controller (+/-)
int pid_max_roll = 400;                    

//Gain settings for the pitch PID controllers
float pid_p_gain_pitch = pid_p_gain_roll;  
float pid_i_gain_pitch = pid_i_gain_roll;  
float pid_d_gain_pitch = pid_d_gain_roll;  
//Maximum output of the PID-controller (+/-)
int pid_max_pitch = pid_max_roll;          

//Gain settings for the yaw PID controllers
float pid_p_gain_yaw = 4.0;                
float pid_i_gain_yaw = 0.02;               
float pid_d_gain_yaw = 0.0;                
//Maximum output of the PID-controller (+/-)
int pid_max_yaw = 400;                     

//Auto leveling of quadcopter
boolean auto_level = true;                 


//Global variables

byte last_channel_1, last_channel_2, last_channel_3, last_channel_4;
byte eeprom_data[36];
byte highByte, lowByte;
volatile int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4;
int counter_channel_1, counter_channel_2, counter_channel_3, counter_channel_4, loop_counter;
int esc_1, esc_2, esc_3, esc_4;
int throttle, battery_voltage;
int cal_int, start, gyro_address;
int receiver_input[5];
int temperature;
int acc_axis[4], gyro_axis[4];
float roll_level_adjust, pitch_level_adjust;

long acc_x, acc_y, acc_z, acc_total_vector;
unsigned long timer_channel_1, timer_channel_2, timer_channel_3, timer_channel_4, esc_timer, esc_loop_timer;
unsigned long timer_1, timer_2, timer_3, timer_4, current_time;
unsigned long loop_timer;
double gyro_pitch, gyro_roll, gyro_yaw;
double gyro_axis_cal[4];
float pid_error_temp;
float pid_i_mem_roll, pid_roll_setpoint, gyro_roll_input, pid_output_roll, pid_last_roll_d_error;
float pid_i_mem_pitch, pid_pitch_setpoint, gyro_pitch_input, pid_output_pitch, pid_last_pitch_d_error;
float pid_i_mem_yaw, pid_yaw_setpoint, gyro_yaw_input, pid_output_yaw, pid_last_yaw_d_error;
float angle_roll_acc, angle_pitch_acc, angle_pitch, angle_roll;
boolean gyro_angles_set;


//Setup routine

void setup(){
  //Copy the EEPROM data for fast access to the data
  for(start = 0; start <= 35; start++)eeprom_data[start] = EEPROM.read(start);
  //Set start back to zero
  start = 0;                                                                
  //Store the gyro address in the variable
  gyro_address = eeprom_data[32];                                           

  //Start the I2C as master
  Wire.begin();            
                                                   
  //Set the I2C clock speed to 400kHz
  TWBR = 12;                                                                

  //Configure digital port 4, 5, 6 and 7 as output
  DDRD |= B11110000;                                                        
  //Configure digital port 4, 5, 6 and 7 as output
  DDRB |= B00110000;                                                        

  //LED indicates startup
  digitalWrite(12,HIGH);                                                    

  //Check the EEPROM signature to make sure that the setup program is executed.
  while(eeprom_data[33] != 'J' || eeprom_data[34] != 'M' || eeprom_data[35] != 'B')delay(10);

  //Check to ensure that the MPU-6050 gyro/accelerometer is connected or program will terminate  
  if(eeprom_data[31] == 2 || eeprom_data[31] == 3)delay(10);

  //Set the gyro registers
  set_gyro_registers();                                                     

  for (cal_int = 0; cal_int < 1250 ; cal_int ++){                           
    //Set the digital ports 4, 5, 6 and 7 high positions
    PORTD |= B11110000;                                                     
    delayMicroseconds(1000);
    //Set digital ports 4, 5, 6 and 7 low positions                                                
    PORTD &= B00001111;                                                     
    delayMicroseconds(3000);                                                
  }

  //Reading several data samples to check the average offset for the gyro for calibration
  for (cal_int = 0; cal_int < 2000 ; cal_int ++){
    //LED indicates calibration                           
    if(cal_int % 15 == 0)digitalWrite(12, !digitalRead(12));
    //Read the gyro output                
    gyro_signals();     
    //Add roll value to gyro_roll_cal                                                    
    gyro_axis_cal[1] += gyro_axis[1];
    //Add pitch value to gyro_pitch_cal                                       
    gyro_axis_cal[2] += gyro_axis[2];
    //Add yaw value to gyro_yaw_cal                                       
    gyro_axis_cal[3] += gyro_axis[3];                                       
  
    //Set the digital ports 4, 5, 6 and 7 high positions
    PORTD |= B11110000;
    //ESCs given 1000 micro second pulses whilst calibrating                                                     
    delayMicroseconds(1000);
    //Set digital ports 4, 5, 6 and 7 low positions                                                
    PORTD &= B00001111;                                                     
    delay(3);                                                               
  }
  
  //Calculating the gyro offset
  gyro_axis_cal[1] /= 2000;                                                 
  gyro_axis_cal[2] /= 2000;                                                 
  gyro_axis_cal[3] /= 2000;                                                 

  //Set PCIE0 to enable an interrupt scan (PCMSK0)
  PCICR |= (1 << PCIE0);
  //Set PCINT0 - PCINT3 (digital input 8 - 11) to trigger an interrupt on state change                                                    
  PCMSK0 |= (1 << PCINT0);                                                  
  PCMSK0 |= (1 << PCINT1);                                                  
  PCMSK0 |= (1 << PCINT2);                                                  
  PCMSK0 |= (1 << PCINT3);                                                  

  //Waiting for reciever to be active and throttle in lowest position
  while(receiver_input_channel_3 < 990 || receiver_input_channel_3 > 1020 || receiver_input_channel_4 < 1400){
    //Convert the actual receiver signals for throttle to the standard 1000 - 2000us
    receiver_input_channel_3 = convert_receiver_channel(3);                 
    //Convert the actual receiver signals for yaw to the standard 1000 - 2000us
    receiver_input_channel_4 = convert_receiver_channel(4);                 
    start ++;                                               
                    
    //Set the digital ports 4, 5, 6 and 7 high positions
    PORTD |= B11110000; 
    //ESCs given 1000 micro second pulses whilst waiting for reciever signals                                                    
    delayMicroseconds(1000);
    //Set digital ports 4, 5, 6 and 7 low positions                                                
    PORTD &= B00001111;                                                     
    delay(3);
    //Every 125 loops (500ms)                                                               
    if(start == 125){
      //Change the LED status
      digitalWrite(12, !digitalRead(12));                                   
      //Start again at 0
      start = 0;                                                            
    }
  }
  //Set start back to 0
  start = 0;                                                                

  //Load the battery voltage to the battery_voltage variable.
  //65 is the voltage compensation for the diode.
  //12.6V equals ~5V @ Analog 0.
  //12.6V equals 1023 analogRead(0).
  //1260 / 1023 = 1.2317.
  //The variable battery_voltage holds 1050 if the battery voltage is 10.5V.
  battery_voltage = (analogRead(0) + 65) * 1.2317;

  //Set the timer for the next loop
  loop_timer = micros();                                                    

  //Turn off the warning LED
  digitalWrite(12,LOW);                                                     
}


//Main program loop
void loop(){

  //Gyro pid input is deg/sec
  //65.5 = 1 deg/sec
  gyro_roll_input = (gyro_roll_input * 0.7) + ((gyro_roll / 65.5) * 0.3);   
  gyro_pitch_input = (gyro_pitch_input * 0.7) + ((gyro_pitch / 65.5) * 0.3);
  gyro_yaw_input = (gyro_yaw_input * 0.7) + ((gyro_yaw / 65.5) * 0.3);      
  
  //Calculating the gyro angles
  //0.0000611 = 1 / (250Hz / 65.5)
  //Calculate the travelled pitch angle and add this to the angle_pitch variable
  angle_pitch += gyro_pitch * 0.0000611;                                    
  //Calculate the travelled roll angle and add this to the angle_roll variable
  angle_roll += gyro_roll * 0.0000611;                                      

  //0.000001066 = 0.0000611 * (3.142(PI) / 180degr)
  //If the IMU has yawed transfer the roll angle to the pitch angle
  angle_pitch -= angle_roll * sin(gyro_yaw * 0.000001066);                  
  //If the IMU has yawed transfer the pitch angle to the roll angle
  angle_roll += angle_pitch * sin(gyro_yaw * 0.000001066);                  

  //Accelerometer angle calculations
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));       

  //Calculate the pitch angle
  if(abs(acc_y) < acc_total_vector){                                        
    angle_pitch_acc = asin((float)acc_y/acc_total_vector)* 57.296;          
  }
  //Calculate the roll angle
  if(abs(acc_x) < acc_total_vector){                                        
    angle_roll_acc = asin((float)acc_x/acc_total_vector)* -57.296;          
  }

  //Accelerometer calibration value for pitch and roll
  angle_pitch_acc -= 0.0;                                                   
  angle_roll_acc -= 0.0;                                                    

  //Correcting the drift of the pitch and roll angles through the accelerometer
  angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;            
  angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;               

  //Calculate the pitch angle and roll angle correction
  pitch_level_adjust = angle_pitch * 15;                                    
  roll_level_adjust = angle_roll * 15;                                      

  if(!auto_level){                                                          
  //Set the pitch angle and roll angle correction to zero
    pitch_level_adjust = 0;                                                 
    roll_level_adjust = 0;                                                  
  }


  //For starting the motors: throttle low and yaw left 
  if(receiver_input_channel_3 < 1050 && receiver_input_channel_4 < 1050)start = 1;
  //When yaw stick is back in the center position the motor will start
  if(start == 1 && receiver_input_channel_3 < 1050 && receiver_input_channel_4 > 1450){
    start = 2;

  //Set the gyro pitch angle equal to the accelerometer pitch angle when the quadcopter is started
    angle_pitch = angle_pitch_acc;           
    //Set the gyro roll angle equal to the accelerometer roll angle when the quadcopter is started                               
    angle_roll = angle_roll_acc;                                      
    //Set the IMU started flag      
    gyro_angles_set = true;                                                 

    //Reset the PID controllers
    pid_i_mem_roll = 0;
    pid_last_roll_d_error = 0;
    pid_i_mem_pitch = 0;
    pid_last_pitch_d_error = 0;
    pid_i_mem_yaw = 0;
    pid_last_yaw_d_error = 0;
  }
  //Stopping the motors: throttle low and yaw right
  if(start == 2 && receiver_input_channel_3 < 1050 && receiver_input_channel_4 > 1950)start = 0;

  //The PID set point in degrees per second is determined by the roll receiver input
  //In the case of deviding by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s )
  pid_roll_setpoint = 0;
  
  if(receiver_input_channel_1 > 1508)pid_roll_setpoint = receiver_input_channel_1 - 1508;
  else if(receiver_input_channel_1 < 1492)pid_roll_setpoint = receiver_input_channel_1 - 1492;

  //Subtract the angle correction from the standardized receiver roll input value
  pid_roll_setpoint -= roll_level_adjust;                                   
  //Divide the setpoint for the PID roll controller by 3 to get angles in degrees
  pid_roll_setpoint /= 3.0;                                                 


  //The PID set point in degrees per second is determined by the pitch receiver input
  //In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s )
  pid_pitch_setpoint = 0;
  
  if(receiver_input_channel_2 > 1508)pid_pitch_setpoint = receiver_input_channel_2 - 1508;
  else if(receiver_input_channel_2 < 1492)pid_pitch_setpoint = receiver_input_channel_2 - 1492;

  //Subtract the angle correction from the standardized receiver pitch input value
  pid_pitch_setpoint -= pitch_level_adjust;                                  
  //Divide the setpoint for the PID pitch controller by 3 to get angles in degrees
  pid_pitch_setpoint /= 3.0;                                                 

  //The PID set point in degrees per second is determined by the yaw receiver input
  //In the case of deviding by 3 the max yaw rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s )
  pid_yaw_setpoint = 0;
  
  if(receiver_input_channel_3 > 1050){ 
    if(receiver_input_channel_4 > 1508)pid_yaw_setpoint = (receiver_input_channel_4 - 1508)/3.0;
    else if(receiver_input_channel_4 < 1492)pid_yaw_setpoint = (receiver_input_channel_4 - 1492)/3.0;
  }
  
  //Calculate the PID outputs from the PID inputs
  calculate_pid();                                                            

  //The battery voltage is needed for compensation
  //0.09853 = 0.08 * 1.2317
  battery_voltage = battery_voltage * 0.92 + (analogRead(0) + 65) * 0.09853;

  //Turn on the LED if battery voltage is to low
  if(battery_voltage < 1000 && battery_voltage > 600)digitalWrite(12, HIGH);

  //Throttle signal set as a base signal
  throttle = receiver_input_channel_3;                                      

  if (start == 2){                                                          
    if (throttle > 1800) throttle = 1800;
    //Calculate the pulse for all ESCs                                   
    esc_1 = throttle - pid_output_pitch + pid_output_roll - pid_output_yaw; 
    esc_2 = throttle + pid_output_pitch + pid_output_roll + pid_output_yaw; 
    esc_3 = throttle + pid_output_pitch - pid_output_roll - pid_output_yaw; 
    esc_4 = throttle - pid_output_pitch - pid_output_roll + pid_output_yaw; 

    //Compensate the ESCs in case of a drop in voltage
    if (battery_voltage < 1240 && battery_voltage > 800){                   
      esc_1 += esc_1 * ((1240 - battery_voltage)/(float)3500);              
      esc_2 += esc_2 * ((1240 - battery_voltage)/(float)3500);              
      esc_3 += esc_3 * ((1240 - battery_voltage)/(float)3500);              
      esc_4 += esc_4 * ((1240 - battery_voltage)/(float)3500);              
    } 

    //Keep all motors running
    if (esc_1 < 1100) esc_1 = 1100;                                         
    if (esc_2 < 1100) esc_2 = 1100;                                         
    if (esc_3 < 1100) esc_3 = 1100;                                         
    if (esc_4 < 1100) esc_4 = 1100;                                         

  //Limit the upper boundry to 2000us pulse
    if(esc_1 > 2000)esc_1 = 2000;                                           
    if(esc_2 > 2000)esc_2 = 2000;                                           
    if(esc_3 > 2000)esc_3 = 2000;                                           
    if(esc_4 > 2000)esc_4 = 2000;                                           
  }

  // If the start value is not 2 set a 1000us pulse for all ESCs
  else{
    esc_1 = 1000;                                                           
    esc_2 = 1000;                                                           
    esc_3 = 1000;                                                           
    esc_4 = 1000;                                                           
  }


  //Turn on the LED if the loop time exceeds 4050us
  if(micros() - loop_timer > 4050)digitalWrite(12, HIGH);                   
  
  //The refresh rate is 250Hz/4ms pulse
  while(micros() - loop_timer < 4000);                                      
  loop_timer = micros();                                                    

  //Set digital outputs 4,5,6 and 7 high
  PORTD |= B11110000;                 
  //Calculate the time of the faling edge of the ESCs pulse                                      
  timer_channel_1 = esc_1 + loop_timer;                                     
  timer_channel_2 = esc_2 + loop_timer;                                     
  timer_channel_3 = esc_3 + loop_timer;                                     
  timer_channel_4 = esc_4 + loop_timer;                                     
  

  //Gets the current gyro and receiver data and scale it to degrees per second for the PID calculations
  gyro_signals();

  while(PORTD >= 16){                                                       
    esc_loop_timer = micros();                   
    //Sets the digital output of 4 - 7 to low if the time is expired                           
    if(timer_channel_1 <= esc_loop_timer)PORTD &= B11101111;                
    if(timer_channel_2 <= esc_loop_timer)PORTD &= B11011111;                
    if(timer_channel_3 <= esc_loop_timer)PORTD &= B10111111;                
    if(timer_channel_4 <= esc_loop_timer)PORTD &= B01111111;                
  }
}

ISR(PCINT0_vect){
  current_time = micros();
  //Channel 1
  //Checks the state of input 8
  if(PINB & B00000001){                                                     
    if(last_channel_1 == 0){                                                
      last_channel_1 = 1;
      //Stores the current time                                                   
      timer_1 = current_time;                                               
    }
  }
  else if(last_channel_1 == 1){                                             
    last_channel_1 = 0; 
    //Channel 1 is the time difference between the last reading and the present one                                                   
    receiver_input[1] = current_time - timer_1;                             
  }
  
  //Channel 2
  if(PINB & B00000010 ){                                                    
    if(last_channel_2 == 0){                                                
      last_channel_2 = 1;                                                   
      timer_2 = current_time;                                               
    }
  }
  else if(last_channel_2 == 1){                                             
    last_channel_2 = 0;                                                     
    receiver_input[2] = current_time - timer_2;                             
  }
  
  //Channel 3
  if(PINB & B00000100 ){                                                    
    if(last_channel_3 == 0){                                                
      last_channel_3 = 1;                                                   
      timer_3 = current_time;                                               
    }
  }
  else if(last_channel_3 == 1){                                             
    last_channel_3 = 0;                                                     
    receiver_input[3] = current_time - timer_3;                             

  }
  
  //Channel 4
  if(PINB & B00001000 ){                                                    
    if(last_channel_4 == 0){                                                
      last_channel_4 = 1;                                                   
      timer_4 = current_time;                                               
    }
  }
  else if(last_channel_4 == 1){                                             
    last_channel_4 = 0;                                                     
    receiver_input[4] = current_time - timer_4;                             
  }
}



//Subroutine for reading the gyro
void gyro_signals(){
  //Read the MPU-6050
  if(eeprom_data[31] == 1){
    //Start reading at register 43h and increments with every read
    Wire.beginTransmission(gyro_address);                                   
    Wire.write(0x3B);                                                       
    //End the transmission
    Wire.endTransmission();             
    //Request 14 bytes from the gyro                                    
    Wire.requestFrom(gyro_address,14);                                      

    //Convert the actual receiver signals for pitch, roll, throttle and yaw to the standard 1000 - 2000us
    receiver_input_channel_1 = convert_receiver_channel(1);                 
    receiver_input_channel_2 = convert_receiver_channel(2);                 
    receiver_input_channel_3 = convert_receiver_channel(3);                 
    receiver_input_channel_4 = convert_receiver_channel(4);                 

    //Waits to recieve data from gyro
    while(Wire.available() < 14);    
    //Add the low and high byte to the acc_x variable                                       
    acc_axis[1] = Wire.read()<<8|Wire.read();                               
    //Add the low and high byte to the acc_y variable
    acc_axis[2] = Wire.read()<<8|Wire.read();                               
    //Add the low and high byte to the acc_z variable
    acc_axis[3] = Wire.read()<<8|Wire.read();                               
    //Add the low and high byte to the temperature variable
    temperature = Wire.read()<<8|Wire.read();                               
    //Read high and low part of the angular data
    gyro_axis[1] = Wire.read()<<8|Wire.read();                              
    //Read high and low part of the angular data
    gyro_axis[2] = Wire.read()<<8|Wire.read();                            
    //Read high and low part of the angular data  
    gyro_axis[3] = Wire.read()<<8|Wire.read();                             
  }

  if(cal_int == 2000){
    //Compensates after the calibration
    gyro_axis[1] -= gyro_axis_cal[1];                                       
    gyro_axis[2] -= gyro_axis_cal[2];                                       
    gyro_axis[3] -= gyro_axis_cal[3];                                       
  }
  //Set gyro_roll to the axis stored in the EEPROM
  gyro_roll = gyro_axis[eeprom_data[28] & 0b00000011];         
  //Invert gyro_roll if the MSB of EEPROM bit 28 is set             
  if(eeprom_data[28] & 0b10000000)gyro_roll *= -1;                          
  //Set gyro_pitch to the axis stored in the EEPROM
  gyro_pitch = gyro_axis[eeprom_data[29] & 0b00000011];                     
  //Invert gyro_pitch if the MSB of EEPROM bit 29 is set
  if(eeprom_data[29] & 0b10000000)gyro_pitch *= -1;                         
  //Set gyro_yaw to the axis stored in the EEPROM
  gyro_yaw = gyro_axis[eeprom_data[30] & 0b00000011];                       
  //Invert gyro_yaw if the MSB of EEPROM bit 30 is set
  if(eeprom_data[30] & 0b10000000)gyro_yaw *= -1;                           

  //Set acc_x to the axis stored in the EEPROM
  acc_x = acc_axis[eeprom_data[29] & 0b00000011];                           
  //Invert acc_x if the MSB of EEPROM bit 29 is set
  if(eeprom_data[29] & 0b10000000)acc_x *= -1;                              
  //Set acc_y to the axis stored in the EEPROM
  acc_y = acc_axis[eeprom_data[28] & 0b00000011];                           
  //Invert acc_y if the MSB of EEPROM bit 28 is set
  if(eeprom_data[28] & 0b10000000)acc_y *= -1;                              
  //Set acc_z to the axis stored in the EEPROM
  acc_z = acc_axis[eeprom_data[30] & 0b00000011];                           
  //Invert acc_z if the MSB of EEPROM bit 30 is set
  if(eeprom_data[30] & 0b10000000)acc_z *= -1;                              
}


//Subroutine for calculating pid outputs
void calculate_pid(){
  //Roll calculations
  pid_error_temp = gyro_roll_input - pid_roll_setpoint;
  pid_i_mem_roll += pid_i_gain_roll * pid_error_temp;
  if(pid_i_mem_roll > pid_max_roll)pid_i_mem_roll = pid_max_roll;
  else if(pid_i_mem_roll < pid_max_roll * -1)pid_i_mem_roll = pid_max_roll * -1;

  pid_output_roll = pid_p_gain_roll * pid_error_temp + pid_i_mem_roll + pid_d_gain_roll * (pid_error_temp - pid_last_roll_d_error);
  if(pid_output_roll > pid_max_roll)pid_output_roll = pid_max_roll;
  else if(pid_output_roll < pid_max_roll * -1)pid_output_roll = pid_max_roll * -1;

  pid_last_roll_d_error = pid_error_temp;

  //Pitch calculations
  pid_error_temp = gyro_pitch_input - pid_pitch_setpoint;
  pid_i_mem_pitch += pid_i_gain_pitch * pid_error_temp;
  if(pid_i_mem_pitch > pid_max_pitch)pid_i_mem_pitch = pid_max_pitch;
  else if(pid_i_mem_pitch < pid_max_pitch * -1)pid_i_mem_pitch = pid_max_pitch * -1;

  pid_output_pitch = pid_p_gain_pitch * pid_error_temp + pid_i_mem_pitch + pid_d_gain_pitch * (pid_error_temp - pid_last_pitch_d_error);
  if(pid_output_pitch > pid_max_pitch)pid_output_pitch = pid_max_pitch;
  else if(pid_output_pitch < pid_max_pitch * -1)pid_output_pitch = pid_max_pitch * -1;

  pid_last_pitch_d_error = pid_error_temp;

  //Yaw calculations
  pid_error_temp = gyro_yaw_input - pid_yaw_setpoint;
  pid_i_mem_yaw += pid_i_gain_yaw * pid_error_temp;
  if(pid_i_mem_yaw > pid_max_yaw)pid_i_mem_yaw = pid_max_yaw;
  else if(pid_i_mem_yaw < pid_max_yaw * -1)pid_i_mem_yaw = pid_max_yaw * -1;

  pid_output_yaw = pid_p_gain_yaw * pid_error_temp + pid_i_mem_yaw + pid_d_gain_yaw * (pid_error_temp - pid_last_yaw_d_error);
  if(pid_output_yaw > pid_max_yaw)pid_output_yaw = pid_max_yaw;
  else if(pid_output_yaw < pid_max_yaw * -1)pid_output_yaw = pid_max_yaw * -1;

  pid_last_yaw_d_error = pid_error_temp;
}

//Converts the actual receiver signals to a standardized 1000 – 1500 – 2000 microsecond values
//The stored data in the EEPROM is used
int convert_receiver_channel(byte function){
  byte channel, reverse;                                                       
  int low, center, high, actual;
  int difference;

  //Channel corresponds with the specific function
  channel = eeprom_data[function + 23] & 0b00000111;                      
  //Reverse channel when most significant bit is set     
  if(eeprom_data[function + 23] & 0b10000000)reverse = 1;                      
  else reverse = 0;                                                            

  //Read the actual receiver value for the corresponding function
  actual = receiver_input[channel];                                            
  //Store the low value for the specific receiver input channel
  low = (eeprom_data[channel * 2 + 15] << 8) | eeprom_data[channel * 2 + 14];  
  //Store the center value for the specific receiver input channel
  center = (eeprom_data[channel * 2 - 1] << 8) | eeprom_data[channel * 2 - 2]; 
  //Store the high value for the specific receiver input channel
  high = (eeprom_data[channel * 2 + 7] << 8) | eeprom_data[channel * 2 + 6];

  //The actual receiver value is lower than the center value
  if(actual < center){                                                         
    //Limit the lowest value to the value that was detected during setup
    if(actual < low)actual = low;                                              
    //Calculate and scale the actual value to a 1000 - 2000us value
    difference = ((long)(center - actual) * (long)500) / (center - low);       
    if(reverse == 1)return 1500 + difference;                                  
    else return 1500 - difference;                                             
  }
  //The actual receiver value is higher than the center value
  else if(actual > center){                                                    
    //Limit the lowest value to the value that was detected during setup                    
    if(actual > high)actual = high;                                            
    //Calculate and scale the actual value to a 1000 - 2000us value
    difference = ((long)(actual - center) * (long)500) / (high - center);      
    if(reverse == 1)return 1500 - difference;                                  
    else return 1500 + difference;                                             
  }
  else return 1500;
}

void set_gyro_registers(){
  //Setup the MPU-6050
  if(eeprom_data[31] == 1){
    //Start communication with the address found during search
    Wire.beginTransmission(gyro_address);           
    //Writing to the PWR_MGMT_1 register (6B hex)                           
    Wire.write(0x6B);                                                          
    //Set the register bits as 00000000 to activate the gyro
    Wire.write(0x00);                                                          
    Wire.endTransmission();                                                    

    Wire.beginTransmission(gyro_address);                                      
    //Write to the GYRO_CONFIG register (1B hex)
    Wire.write(0x1B);                                                          
    //Set the register bits as 00001000 (500dps full scale)
    Wire.write(0x08);                                                          
    Wire.endTransmission();                                                    

    Wire.beginTransmission(gyro_address);                                      
    //Write to the ACCEL_CONFIG register (1A hex)
    Wire.write(0x1C);                                                          
    //Set the register bits as 00010000 (+/- 8g full scale range)
    Wire.write(0x10);                                                          
    Wire.endTransmission();                                                    

    //Check to see if the values are correct
    Wire.beginTransmission(gyro_address);                                      
    //Start reading @ register 0x1B
    Wire.write(0x1B);                                                          
    //End the transmission
    Wire.endTransmission();                                                    
    //Request 1 bytes from the gyro
    Wire.requestFrom(gyro_address, 1);                                         
    //Wait until the 6 bytes are received
    while(Wire.available() < 1);                                               
    //Check if the value is 0x08
    if(Wire.read() != 0x08){                                                   
      //Turn on the warning led
      digitalWrite(12,HIGH);                                                   
      while(1)delay(10);                                                       
    }

    Wire.beginTransmission(gyro_address);                                      
    //We want to write to the CONFIG register (1A hex)
    Wire.write(0x1A);                                                          
    //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz)
    Wire.write(0x03);                                                          
    Wire.endTransmission();                                                        

  }  
}

