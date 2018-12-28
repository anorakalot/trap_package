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


 enum sensor_STATES{sensor_START,sensor_INIT,sensor_SENSE}sensor_state;

 void sensor_init(){
	 sensor_state = sensor_START;
 }


 void sensor_tick(){

	 switch(sensor_state){

		
		 case sensor_START:
		 sensor_state = sensor_INIT;
		 break;
		 
		 case sensor_INIT:
		 sensor_state = sensor_SENSE;
		 break;


		 case sensor_SENSE:
		 sensor_state = sensor_SENSE;
		 break;

		 
	 }
	 switch(sensor_state){
		 case sensor_START:
		 break;

		 case sensor_INIT:
		 break;

		 case sensor_SENSE:
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


 enum stepper_STATES{stepper_START,stepper_INIT,stepper_SENSE}stepper_state;

 void stepper_init(){
	 stepper_state = stepper_START;
 }


 void stepper_tick(){

	 switch(stepper_state){
		 case stepper_START:
		 stepper_state = stepper_INIT;
		 break;
		 
		 case stepper_INIT:
		 stepper_state = stepper_SENSE;
		 break;


		 case stepper_SENSE:
		 stepper_state = stepper_SENSE;
		 break;

		 
	 }
	 switch(stepper_state){
		 case stepper_START:
		 break;

		 case stepper_INIT:
		 break;

		 case stepper_SENSE:
		 break;

	 }
 }

 void stepper_task(){
	 stepper_init();
	 for(;;){
		 stepper_tick();
		 vTaskDelay(10);
	 }
 }


 void StartSecPulse2(unsigned portBASE_TYPE Priority)
 {
	 xTaskCreate(stepper_task,(signed portCHAR *)"stepper_task",configMINIMAL_STACK_SIZE,NULL,Priority,NULL);
}



int main(void)
{
StartSecPulse(1);
StartSecPulse2(1);
vTaskStartScheduler();

return 0;
}