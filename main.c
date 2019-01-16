/*
 * trap_package.c
 *
 * Created: 12/26/2018 11:30:22 AM
 * Author : Dylan
 */ 
 #define F_CPU 8000000UL
 #include <stdint.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <string.h>
 #include <math.h>
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <avr/eeprom.h>
 #include <avr/portpins.h>
 #include <avr/pgmspace.h>
 //FreeRTOS include files
 #include "FreeRTOS.h"
 #include "task.h"
 #include "croutine.h"
 #include "util/delay.h"
 #include "io.c" //for lcd screen

//for sensors 
 static volatile int pulse = 0;
 static volatile int i = 0;
 int16_t count_a = 0;
 char show_a[16] = "testing";
 int16_t distance_reading = 0;


 //for stepper motors
  unsigned char stepper_phases[8] = {0x01,0x03,0x02,0x06,0x04,0x0C,0x08,0x09};
  unsigned char stepper_index;
  unsigned long degree_180_turn = (180/5.625) * 64;
  unsigned long curr_num_of_turns = 0;


 //maybe not need this
 void ADC_init() {
	 ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	 // ADEN: setting this bit enables analog-to-digital conversion.
	 // ADSC: setting this bit starts the first conversion.
	 // ADATE: setting this bit enables auto-triggering. Since we are
	 //        in Free Running Mode, a new conversion will trigger whenever
	 //        the previous conversion completes.
 }
 //this is for the sensors
 ISR(INT0_vect){
 if (i == 1){
	TCCR1B = 0;
	pulse = TCNT1;
	TCNT1 = 0;
	i = 0;
 }
 if (i == 0){
	TCCR1B = 0x01;
	i = 1;
 }
}



 enum sensor_STATES{sensor_START,sensor_INIT,sensor_SENSE}sensor_state;

 void sensor_init(){
	 sensor_state = sensor_START;
 }


 void sensor_tick(){

	 switch(sensor_state){//transitions		

		 case sensor_START:
		 sensor_state = sensor_INIT;
		 break;

		 case sensor_INIT:
		 sensor_state = sensor_SENSE;
		 break;


		 case sensor_SENSE:
		 sensor_state = sensor_SENSE;
		 break;

		 default:
		 sensor_state = sensor_START;
	     break;		 

	 }

	 switch(sensor_state){ // actions
		 case sensor_START:
		 break;

		 case sensor_INIT:
		 //enables the interuups
		 //GICR |= 1<< INT0;
		 EICRA = 0x01;
		 //MCUCR |= 1<< ISC00;
		 break;

		 case sensor_SENSE:

		 PORTD = (PORTD & 0xFE) | 0x01;
		 delay_ms(15);
		 PORTD = (PORTD & 0xFE) | 0x00;
		 count_a = pulse / 58;
		 distance_reading = count_a;
		 break;
	 }
 }

 


 void sensor_task(){
	 sensor_init();
	 for(;;){
		 sensor_tick();
		 vTaskDelay(10);
	 }
 }


 void StartSecPulse(unsigned portBASE_TYPE Priority)
 {
	 xTaskCreate(sensor_task,(signed portCHAR *)"sensor_task",configMINIMAL_STACK_SIZE,NULL,Priority,NULL);
 }





 enum stepper_STATES{stepper_START,stepper_INIT,stepper_WAIT,stepper_ON,stepper_OFF}stepper_state;

 void stepper_init(){
	 stepper_state = stepper_START;
 }


 void stepper_tick(){

	 switch(stepper_state){//transitions
		 case stepper_START:
		 stepper_state = stepper_INIT;
		 break;
		 
		 case stepper_INIT:
		 stepper_state = stepper_WAIT;
		 break;

		 case stepper_WAIT:
		 stepper_state = stepper_ON;
		 break;

		 case stepper_ON:
		 stepper_state = stepper_ON;
		 break;
		 
		 case stepper_OFF:
		 stepper_state = stepper_WAIT;
		 break;

		 default:
		 stepper_state = stepper_START;
		 break;
	 }
	 switch(stepper_state){//actions
		 case stepper_START:
		 break;

		 case stepper_INIT:
		 stepper_index = 0;
		 curr_num_of_turns = 0;
		 break;

		 case stepper_WAIT:
		 PORTB = (PORTB & 0xF0) | stepper_phases[stepper_index];
		 break;

		 case stepper_ON:
		 if (stepper_index > 7){
		 stepper_index = 0;
		 }
		 PORTB = (PORTB & 0xF0) | stepper_phases[stepper_index];
		 stepper_index+= 1;
		 break;

		 case stepper_OFF:
		 PORTB = (PORTB & 0xF0) | stepper_phases[stepper_index];
		 break;

	 }
 }

 void stepper_task(){
	 stepper_init();
	 for(;;){
		 stepper_tick();
		 vTaskDelay(1);
	 }
}


 void StartSecPulse2(unsigned portBASE_TYPE Priority)
 {
	 xTaskCreate(stepper_task,(signed portCHAR *)"stepper_task",configMINIMAL_STACK_SIZE,NULL,Priority,NULL);
}

 enum LCD_STATES{LCD_START,LCD_INIT,LCD_SHOW}LCD_state;

 void LCD_init_state(){
	 LCD_state = LCD_START;
 }


 void LCD_tick(){
	 switch(LCD_state){//transitions
		 case LCD_START:
		 LCD_state = LCD_INIT;
		 break;
		 
		 case LCD_INIT:
		 LCD_state = LCD_SHOW;
		 break;


		 case LCD_SHOW:
		 LCD_state = LCD_SHOW;
		 break;

		 default:
		 LCD_state = LCD_START;
		 break;
	 }
	 switch(LCD_state){ // actions
		 case LCD_START:
		 break;

		 case LCD_INIT:
		 LCD_init();
		 break;

		 case LCD_SHOW:
		 LCD_ClearScreen();
		 itoa(count_a,show_a,10);
		 LCD_DisplayString(1,show_a);
		 break;
	 }
 }

 


 void LCD_task(){
	 LCD_init_state();
	 for(;;){
		 LCD_tick();
		 vTaskDelay(500);
	 }
 }


 void StartSecPulse3(unsigned portBASE_TYPE Priority)
 {
	 xTaskCreate(LCD_task,(signed portCHAR *)"LCD_task",configMINIMAL_STACK_SIZE,NULL,Priority,NULL);
 }




 enum dc_motor_STATES{dc_motor_START,dc_motor_INIT,dc_motor_WAIT,dc_motor_ON,dc_motor_OFF}dc_motor_state;

 void dc_motor_init(){
	 dc_motor_state = dc_motor_START;
 }


 void dc_motor_tick(){

	 switch(dc_motor_state){//transitions
		 case dc_motor_START:
		 dc_motor_state = dc_motor_INIT;
		 break;
		 
		 case dc_motor_INIT:
		 dc_motor_state = dc_motor_WAIT;
		 break;

		 case dc_motor_WAIT:
		 dc_motor_state = dc_motor_ON;
		 break;

		 case dc_motor_ON:
		 dc_motor_state = dc_motor_ON;
		 break;
		 
		 case dc_motor_OFF:
		 dc_motor_state = dc_motor_WAIT;
		 break;

		 default:
		 dc_motor_state = dc_motor_START;
		 break;
	 }
	 switch(dc_motor_state){//actions
		 case dc_motor_START:
		 break;

		 case dc_motor_INIT:
		 break;

		 //connect this to base of npn transistor
		 case dc_motor_WAIT:
		 PORTA = (PORTA & 0x7F) | 0x80;
		 break;

		 case dc_motor_ON:
		 //makes transistor allow current through
		 PORTA = (PORTA & 0x7F) | 0x80;
		 break;

		 case dc_motor_OFF:
		 //makes transistor not allow current through
		 PORTA = (PORTA & 0x7F) | 0x00;
		 break;

	 }
 }

 void dc_motor_task(){
	 dc_motor_init();
	 for(;;){
		 dc_motor_tick();
		 vTaskDelay(1);
	 }
 }



 void StartSecPulse4(unsigned portBASE_TYPE Priority)
 {
	 xTaskCreate(dc_motor_task,(signed portCHAR *)"dc_motor_task",configMINIMAL_STACK_SIZE,NULL,Priority,NULL);
 }




int main(void)
{
//stepper
PORTB = 0x0F;
//sound sensors
PORTD = 0x04;

//dc motor and lcd
PORTA = 0xFF;

//lcd 
PORTC = 0xFF;


//StartSecPulse(1);
StartSecPulse2(1);
//StartSecPulse3(1);
vTaskStartScheduler();

return 0;
}