//include statements
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include "3pi_32u4_drivers.h"

// Define constants, macros, and mappings
#define LEFT_MOTOR_PWM_LOCATION 6
#define RIGHT_MOTOR_PWM_LOCATION 5
#define LEFT_MOTOR_DIRECTION_LOCATION 2
#define RIGHT_MOTOR_DIRECTION_LOCATION 1
#define PWM_TOP 100
#define INCREMENT 5
#define sensorOneLocation 6
#define sensorTwoLocation 7
#define sensorThreeLocation 5
#define sensorFourLocation 4
#define sensorFiveLocation 1
#define emitterLocation 7

// Configures the robot's emitter
void configure_emitter()
{
	
	DDRB |= (1 << emitterLocation);
	PORTB |= (1 << emitterLocation);
}

// Configures the left motor, allowing for it to be adjusted
void configure_L_Motor(){
	DDRB &= ~(1<< LEFT_MOTOR_PWM_LOCATION);
	PORTB |= (1<< LEFT_MOTOR_PWM_LOCATION);
}

// Configures the right motor, allowing for it to be adjusted
void configure_R_Motor(){
	DDRB &= ~(1<< RIGHT_MOTOR_PWM_LOCATION);
	PORTB |= (1<< RIGHT_MOTOR_PWM_LOCATION);
}

// Configures the left motor's direction
void configure_L_Motor_direction(){
	DDRB &= ~(1<< LEFT_MOTOR_DIRECTION_LOCATION);
	PORTB |= (1<< LEFT_MOTOR_DIRECTION_LOCATION);
}

// Configures the right motor's direction
void configure_R_Motor_direction(){
	DDRB &= ~(1<< RIGHT_MOTOR_DIRECTION_LOCATION);
	PORTB |= (1<< RIGHT_MOTOR_DIRECTION_LOCATION);
}

//set up functions to turn on and off the motors
void turn_on_left_motor()
{
	PORTB |= (1<<LEFT_MOTOR_PWM_LOCATION);
}
void turn_on_right_motor()
{
	PORTB |= (1<<RIGHT_MOTOR_PWM_LOCATION);
}

void turn_off_left_motor()
{
	PORTB &= ~(1<<LEFT_MOTOR_PWM_LOCATION);
}
void turn_off_right_motor()
{
	PORTB &= ~(1<<RIGHT_MOTOR_PWM_LOCATION);
}

//set up functions that can set the direction of the motors
void turn_left_motor_backward()
{
	PORTB |= (1<<LEFT_MOTOR_DIRECTION_LOCATION);
}
void turn_right_motor_backward()
{
	PORTB |= (1<<RIGHT_MOTOR_DIRECTION_LOCATION);
}
void turn_right_motor_forward()
{
	PORTB &= ~(1<<RIGHT_MOTOR_DIRECTION_LOCATION);
}
void turn_left_motor_forward()
{
	PORTB &= ~(1<<LEFT_MOTOR_DIRECTION_LOCATION);
}

// This function allows the user to choose how many mines the robot must find
int minechoose()
{
	//initialize variables that will be used in the loop
	int mines = 0;
	unsigned int last_button_A_state = (button_a_is_up());
	unsigned int button_A_pressed = 0;
	unsigned int last_button_B_state = (button_b_is_up());
	unsigned int button_B_pressed = 0;
	unsigned int last_button_C_state = (button_c_is_up());
	unsigned int button_C_pressed = 0;
	
	while(1)
	{
		//A button pulser; set button_A_pressed to 1 if the button has been
		// pressed in this iteration
		if(button_a_is_up() != last_button_A_state){
			if(button_a_is_up() == 0 ){
				button_A_pressed = 1;
			}
			last_button_A_state = (button_a_is_up());
		}
		else{
			button_A_pressed = 0;
		}
				
		// C button pulser; set button_C_pressed to 1 if the button has been
		// pressed in this iteration
		if(button_c_is_up() != last_button_C_state){
			if(button_c_is_up() == 0 ){
				button_C_pressed = 1;
			}
			last_button_C_state = button_c_is_up();
		}
		else{
			button_C_pressed = 0;
		}
				
		//B button pulser; set button_B_pressed to 1 if the button has been
		// pressed in this iteration
		if(button_b_is_up() != last_button_B_state){
			if(button_b_is_up() == 0 ){
				button_B_pressed = 1;
			}
			last_button_B_state = button_b_is_up();
		}
		else{
			button_B_pressed = 0;
		}
			
		//decrement the mines total if A button is pressed, as low as 0
		if (button_A_pressed == 1 && mines >0 )
		{
			mines--;
		}
		
		//increment the minws total if C button is pressed, up to +25
		if (button_C_pressed == 1 && mines < 25)
		{
			mines++;
		}
				
		//break the loop if B button is pressed
		if (button_B_pressed)
		{
			break;
		}
				
		//Displays the amount of mines that the user entered
		
		LCD_set_cursor(1,0); //Goes to the second line of the screen
		LCD_putchar((char) 77); // Prints 'M' to screen
		LCD_set_cursor(1,1); // Shifts cursor one to right
		LCD_putchar((char) 105); // Prints 'i' to screen
		LCD_set_cursor(1,2); // Shifts cursor one to right
		LCD_putchar((char) 110); // Prints 'n'
		LCD_set_cursor(1,3); // Shifts cursor one to right
		LCD_putchar((char) 101); // Prints 'e'
		LCD_set_cursor(1,4); // Shifts cursor one to right
		LCD_putchar((char) 115); Prints 's'
		LCD_set_cursor(1,5); // Shifts cursor one to right
		LCD_putchar((char) 58); // Displays ':'
		LCD_set_cursor(1,6); // Shifts cursor one to right
		LCD_putchar((char)(abs((mines/10)%10) +48)); // Displays the tens place
		LCD_set_cursor(1,7); // Shifts cursor one to right
		LCD_putchar((char)(abs(mines%10) + 48)); // Displays the ones place
		_delay_us(10); //delay for 10 microseconds
	}
	//return the mines value that was last put in
	return mines;
}

// This function adjusts the bot so that when it turns it will be centered with the line
void TurnPrep( uint32_t prev_left_quad_counter)
{
	//initialize variables, set the motor directions to correspond to a left turn
	uint8_t pwm_counter =0;
	turn_left_motor_forward();
	turn_right_motor_forward();
	_delay_us(10);
	while(1)
	{
		//use a pwm to modulate speed while turning
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // sets counter back to 0 should it reach the maximum pwm
		}
		
		if (pwm_counter < 10) {
			turn_on_left_motor();
			turn_on_right_motor();
			//turn on the motor when the counter is less than duty
			//cycle, which is 10 here.
		}
		else {
			turn_off_left_motor();
			turn_off_right_motor();
			//turn off the motors once it is passed the duty cycle.
		}
		_delay_us(1); // delay for 1 microsecond

		//after the motors run a certain distance, end the turn
		if (get_left_quadrature_counter() > prev_left_quad_counter + 60)
		{
			break; // breaks out of current loop
		}
	}
	//turn off the motors
	turn_off_right_motor();
	turn_off_left_motor();
}

// Function that allows for the robot to perform a left turn
void leftTurn( uint32_t prev_left_quad_counter)
{
	//initialize variables, set the motor directions to correspond to a left turn
	// In this case, the left motor moves back while the right moves forward, shifting the bit to the left
	uint8_t pwm_counter =0;
	turn_left_motor_backward();
	turn_right_motor_forward();
	_delay_us(10);
	while(1)
	{
		//use a pwm to modulate speed while turning
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // sets counter back to 0 should it reach the maximum pwm
		}
		
		if (pwm_counter < 10) {
			//turn on the motor when the counter is less than duty
			//cycle, which is 10 here.
			turn_on_left_motor();
			turn_on_right_motor();
		}
		else {
			//turn off the motors once it is passed the duty cycle.
			turn_off_left_motor();
			turn_off_right_motor();
		}
		_delay_us(1);

		//after the motors run a certain distance, end the turn
		if (get_left_quadrature_counter() > prev_left_quad_counter + 220)
		{
			break;
		}
	}
	//turn off the motors
	turn_off_right_motor();
	turn_off_left_motor();
}

// create a function that enables the bot to turn right
void rightTurn( uint32_t prev_left_quad_counter)
{
	//initialize variables, set the motor directions to correspond to a right turn
	// In this case, right motor goes back, left motor moves forward, tilting the robot right
	uint8_t pwm_counter =0;
	turn_right_motor_backward();
	turn_left_motor_forward();
	_delay_us(10);
	while(1)
	{
		//use a pwm to modulate speed while turning
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // sets counter back to 0 should it reach the maximum pwm
		}
		
		if (pwm_counter < 10) {
			// turns on the motors
			turn_on_left_motor();
			turn_on_right_motor();
		}
		else {
			// turns off the motors
			turn_off_left_motor();
			turn_off_right_motor();
		}
		_delay_us(1); // delay for 1 microsecond
		
		//once the motors get past a certain amount of turning, end the turn
		if (get_left_quadrature_counter() > prev_left_quad_counter + 220)
		{
			break; // breaks out of current while loop
		}
	}
	//turn off the motors
	turn_off_right_motor();
	turn_off_left_motor();
}

void TurnAround( uint32_t prev_left_quad_counter)
{
	//initialize variables, set the motor directions to correspond to a right turn
	//In this case, right motor moves backward, left moves forward
	uint8_t pwm_counter =0;
	turn_right_motor_backward();
	turn_left_motor_forward();
	_delay_us(10); // delay for 10 microseconds
	while(1)
	{
		//use a pwm to modulate speed while turning
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // Resets counter if it reaches maximum value
		}
		
		if (pwm_counter < 10) {
			// turns the motors on
			turn_on_left_motor();
			turn_on_right_motor();
		}
		else {
			// turns off the motors
			turn_off_left_motor();
			turn_off_right_motor();
		}
		_delay_us(1); // delay for 1 microsecond
		
		//once the motors get past a certain amount of turning, end the turn
		if (get_left_quadrature_counter() > prev_left_quad_counter + 440)
		{
			break; // breaks out of current loops
		}
	}
	//turn off the motors
	turn_off_right_motor();
	turn_off_left_motor();
}

// This function allows the bot to perform a 360 degree spin
void fullSpin( uint32_t prev_left_quad_counter)
{
	//initialize variables, set the motor directions to correspond to a right turn
	uint8_t pwm_counter =0;
	turn_right_motor_backward();
	turn_left_motor_forward();
	_delay_us(10); // delay for 10 microseconds
	while(1)
	{
		//use a pwm to modulate speed while turning
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // resets counter if it reaches pwm maximum value
		}
		
		if (pwm_counter < 10) {
			// turns on motors
			turn_on_left_motor();
			turn_on_right_motor();
		}
		else {
			// turn off motors
			turn_off_left_motor();
			turn_off_right_motor();
		}
		_delay_us(1); // delay for 1 microsecond
		
		//once the motors get past a certain amount of turning, end the turn
		if (get_left_quadrature_counter() > prev_left_quad_counter + 920)
		{
			break; // breaks out of current loop
		}
	}
	//turn off the motors
	turn_off_right_motor();
	turn_off_left_motor();
}

// This function displays the amount of mines found
void dispMineCount(int MinesFound)
{
	LCD_set_cursor(0,0); // Goes to the first line of the lcd screen
	LCD_putchar((char) 70); //Displays 'F'
	LCD_set_cursor(0,1); //Shifts one to the right
	LCD_putchar((char) 111); // Displays 'o'
	LCD_set_cursor(0,2); //Shifts one to the right
	LCD_putchar((char) 117); // Displays 'u'
	LCD_set_cursor(0,3); //Shifts one to the right
	LCD_putchar((char) 110); // Displays 'n'
	LCD_set_cursor(0,4); //Shifts one to the right
	LCD_putchar((char) 100); // Displays 'd'
	LCD_set_cursor(0,5); //Shifts one to the right
	LCD_putchar((char) 58); // Displays ':'
	LCD_set_cursor(0,6); //Shifts one to the right
	LCD_putchar((char)(abs((MinesFound/10)%10) +48)); //Displays tens place
	LCD_set_cursor(0,7); //Shifts one to the right
	LCD_putchar((char)(abs(MinesFound%10) + 48)); // Displays ones place
}

// When all of the mines have been found, the bot will perform this function as a celebration
void celebration()
{
	
	LCD_set_cursor(0,0); // sets cursor to first character of the first line
	LCD_putchar((char) 77); // Displays 'M'
	LCD_set_cursor(0,1); //Shifts one to the right
	LCD_putchar((char) 73); // Displays 'I'
	LCD_set_cursor(0,2); //Shifts one to the right
	LCD_putchar((char) 78); // Displays 'N'
	LCD_set_cursor(0,3); //Shifts one to the right
	LCD_putchar((char) 69); // Dispays 'E'
	LCD_set_cursor(0,4); //Shifts one to the right
	LCD_putchar((char) 83); // Displays 'S'
	LCD_set_cursor(0,5); //Shifts one to the right
	LCD_putchar((char) 32); // Displays a space
	LCD_set_cursor(0,6); //Shifts one to the right
	LCD_putchar((char) 32); //Displays a space
	LCD_set_cursor(0,7); //Shifts one to the right
	LCD_putchar((char) 32); //Displays a space
	LCD_set_cursor(1,0); // sets cursor to first character of the next line
	LCD_putchar((char) 68); // Displays 'D'
	LCD_set_cursor(1,1); //Shifts one to the right
	LCD_putchar((char) 69); // Displays 'E'
	LCD_set_cursor(1,2); //Shifts one to the right
	LCD_putchar((char) 70); // Displays 'F'
	LCD_set_cursor(1,3); //Shifts one to the right
	LCD_putchar((char) 85); // Displays 'U'
	LCD_set_cursor(1,4); //Shifts one to the right
	LCD_putchar((char) 83); // Displays 'S'
	LCD_set_cursor(1,5); //Shifts one to the right
	LCD_putchar((char) 69); // Displays 'E'
	LCD_set_cursor(1,6); //Shifts one to the right
	LCD_putchar((char) 68); // Displays 'D'
	LCD_set_cursor(1,7); //Shifts one to the right
	LCD_putchar((char) 33); // Displays '!'
	fullSpin(get_left_quadrature_counter()); // 360 degree spin
	fullSpin(get_left_quadrature_counter());
	
	fullSpin(get_left_quadrature_counter());
}

// When the bot goes off the line to the right, it will shift back to the left onto the line
void shiftleft()
{
	//initialize variables, set the motor directions to correspond to a left turn
	uint8_t pwm_counter =0;
	//turn_left_motor_backward();
	turn_right_motor_forward();
	_delay_us(10); // delay for 10 microseconds
	uint32_t prev_right_quad_counter = get_right_quadrature_counter();
	while(1)
	{
		//use a pwm to modulate speed while turning
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // resets pwm counter
		}
		
		if (pwm_counter < 10) {
			//turn_on_left_motor();
			turn_on_right_motor();
			//turn on the motor when the counter is less than duty
			//cycle, which is 10 here.
		}
		else {
			//turn_off_left_motor();
			turn_off_right_motor();
			//turn off the motors once it is passed the duty cycle.
		}
		_delay_us(1); // delay 1 microsecond

		//after the motors run a certain distance, end the turn
		if (get_right_quadrature_counter() > prev_right_quad_counter + 2)
		{
			break; // breaks out of current loop
		}
	}
	//turn off the motors
	turn_off_right_motor();
	turn_off_left_motor();
}

// When the bot goes off the line to the left, it will shift back to the right onto the line
void shiftright()
{
	//initialize variables, set the motor directions to correspond to a left turn
	uint8_t pwm_counter =0;
	turn_left_motor_forward();
	//turn_right_motor_backward();
	turn_off_right_motor();
	_delay_us(10); // delay for 10 microseconds
	uint32_t prev_left_quad_counter = get_left_quadrature_counter();
	while(1)
	{
		//use a pwm to modulate speed while turning
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // resets pwm counter
		}
		
		if (pwm_counter < 10) {
			turn_on_left_motor();
			//turn_on_right_motor();
			//turn on the motor when the counter is less than duty
			//cycle, which is 10 here.
		}
		else {
			turn_off_left_motor();
			//turn_off_right_motor();
			//turn off the motors once it is passed the duty cycle.
		}
		_delay_us(1); // Delay for 1 microsecond

		//after the motors run a certain distance, end the turn
		if (get_left_quadrature_counter() > prev_left_quad_counter + 2)
		{
			break; // Breaks out of current loop
		}
	}
	//turn off the motors
	turn_off_right_motor();
	turn_off_left_motor();
}

// Whenever the bot finds a mine, it performs the encased sequence
void FindMine(int minesFound)
{
	
	fullSpin(get_left_quadrature_counter());
	turn_off_left_motor();
	turn_off_right_motor();
	dispMineCount(minesFound);
	TurnAround(get_left_quadrature_counter());
}

// Main function
int main(void)
{
	
    //configure the bot with various different functions 
	configure_3pi();
	configure_emitter();
	configure_R_Motor_direction();
	configure_L_Motor_direction();
	configure_R_Motor();
	configure_L_Motor();
	turn_off_left_motor();
	turn_off_right_motor();
	
	uint8_t pwm_counter = 0;
	//use LCD command to prepare for display later
	LCD_simple_command(0x0F);
	// Set the amount of mines needed to be found
	int minecount = minechoose();
	int minesFound = 0;
	int loopTurnCount = 0;
	int loopdirection = 0;
	
	
	//loop 1: Makes the robot find a corner of the board
	while(minesFound!=minecount)
	{
		// Executes encased code if all sensors are detecting dark
		if (line_sensor[0] < 100 && line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100 && line_sensor[4] < 100)
		{
			minesFound++; // increment
			fullSpin(get_left_quadrature_counter()); // Perform 360 spin
			// turns off motors
			turn_off_left_motor();
			turn_off_right_motor();
			// Displays minesFound
			dispMineCount(minesFound);
			// Turns bot 180 degrees
			TurnAround(get_left_quadrature_counter());
			
			// Executes if the amount of mines needed to be found has been reached
			if (minesFound == minecount)
			{
				// Breaks out of current loop
				break;
			}
		}
		// Executs while sensor 1 is detecting light and sensors 2 and 3 are detecting dark
		while(line_sensor[1] < 100 && line_sensor[2] > 100 && line_sensor[3] > 100)
		{
			shiftleft(); // Shifts bot left slightly
		}
		while(line_sensor[1] > 100 && line_sensor[2] > 100 && line_sensor[3] < 100)
		{
			shiftright(); // Shifts bot to right slightly
		}
		//use a pwm to control the speed of the bot
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // Reset pwm counter
		}
		
		if (pwm_counter < 15) {
			// Switches motor directions to forward
			turn_right_motor_forward();
			turn_left_motor_forward();
			
			// Turns on motors
			turn_on_left_motor();
			turn_on_right_motor();
		}
		else {
			// Turns off motors
			turn_off_left_motor();
			turn_off_right_motor();
		}
		// Executes if sensor 0 and 4 are dark while the others are light
		if (line_sensor[0] > 100 && line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100 && line_sensor[4] > 100)
		{
			// Bot will adjust, turn left and add to turn counter
			TurnPrep(get_left_quadrature_counter());
			leftTurn(get_left_quadrature_counter());
			loopTurnCount++;
		}
		// Executes if sensor 0 is dark while the others are light
		if (line_sensor[0] > 100 && line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100 && line_sensor[4] < 100)
		{
			// Bot will adjust, turn right and add to turn counter
			TurnPrep(get_left_quadrature_counter());
			rightTurn(get_left_quadrature_counter());
			loopTurnCount++;
		}
		// Executes if sensor 4 is dark while the others detect a light surface
		if (line_sensor[0] < 100 && line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100 && line_sensor[4] > 100)
		{
			// Bot will adjust, turn left and add to turn counter
			TurnPrep(get_left_quadrature_counter());
			leftTurn(get_left_quadrature_counter());
			loopTurnCount++;
		}
		// If turn count reaches two, it will go back to 0 and it will break out of current loop
		if (loopTurnCount == 2)
		{
			loopTurnCount = 0;
			break;
		}
	}
	
	//loop 2: The bot will make a full revolution on the outside of the grid
	while(minesFound!=minecount)
	{
		
		// Executes if all sensors are sensing a light surface
		if (line_sensor[0] < 100 && line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100 && line_sensor[4] < 100)
		{
			minesFound++; // increment
			fullSpin(get_left_quadrature_counter()); // 360 spin
			// turn off motors
			turn_off_left_motor();
			turn_off_right_motor();
			// Display mines currently found
			dispMineCount(minesFound);
			TurnAround(get_left_quadrature_counter()); // 180 turn
			
			if (minesFound == minecount)
			{	
				break; // breaks out of current loop
			}
			else
			{
				loopTurnCount = 0; // Set turn count to 0
				loopdirection ^= 1; // Switches loop driection
				TurnAround(get_left_quadrature_counter()); // 180 spin
			}
		}
		while(line_sensor[1] < 100 && line_sensor[2] > 100 && line_sensor[3] > 100)
		{
			shiftleft(); // Shifts bot to the left slightly
		}
		while(line_sensor[1] > 100 && line_sensor[2] > 100 && line_sensor[3] < 100)
		{
			shiftright(); // Shifts bot to the right slightly
		}
		//use a pwm to control the speed of the bot
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0; // Resets pwm counter if it reaches maximum value
		}
		
		if (pwm_counter < 12) {
			// Turns both motors forward
			turn_right_motor_forward();
			turn_left_motor_forward();
			// Turns on both motors
			turn_on_left_motor();
			turn_on_right_motor();
		}
		else {
			// Turns off both motors
			turn_off_left_motor();
			turn_off_right_motor();
		}
		// Executes if loopdirection is 0
		if (loopdirection == 0)			
		{
			// Executes if sensors 0 and 2 sense a light surface while sensor 4 detects dark
			if (line_sensor[0] < 100 && line_sensor[2] < 100 && line_sensor[4] > 100)
			{
				TurnPrep(get_left_quadrature_counter()); // Adjusts for turn
				leftTurn(get_left_quadrature_counter()); // Turn left
				loopTurnCount++; // Increase loopTurnCount
			}
			// Executes if loopTurnCount reaches 4
			if (loopTurnCount == 4){
				loopTurnCount = 0; // Resets loopTurnCount
				break; // breaks out of current loop
			}
		}
		else {
			if (line_sensor[4] < 100 && line_sensor[2] < 100 && line_sensor[0] > 100)
			{
				TurnPrep(get_left_quadrature_counter()); // Prep for turn
				rightTurn(get_left_quadrature_counter()); // Right turn
				loopTurnCount++; // Increment to turn count
			}
			// Executes if the value of loopTurnCoutn reaches 4
			if (loopTurnCount == 4){
				loopTurnCount = 0; // Reset turn count
				break; // breaks out of current loop
			}
		}
	}
	
    //loop 3: This portion will allow the bot to search both the horizontal and vertical lines of the grid
	while (minesFound!=minecount)
	{
		while(line_sensor[0] > 100 || line_sensor[4]>100)
		{

			//use a pwm to control the speed of the bot
			pwm_counter = pwm_counter + 1;
			if( pwm_counter >= PWM_TOP ){
				pwm_counter = 0; // Reset pwm counter
			}
			
			if (pwm_counter < 15) {
				// Turns both motors forward
				turn_right_motor_forward();
				turn_left_motor_forward();
				// Turns on both motors
				turn_on_left_motor();
				turn_on_right_motor();
			}
			else {
				// Turns off both motors
				turn_off_left_motor();
				turn_off_right_motor();
			}
			
			// Executes while sensor 1 is detacting a light surface while 2 and 3 detect dark
			while(line_sensor[1] < 100 && line_sensor[2] > 100 && line_sensor[3] > 100)
			{
				shiftleft(); // Shifts bot to the left slightly
			}
			// Executes while sensor 3 is detecting light while 1 and 2 are dark
			while(line_sensor[1] > 100 && line_sensor[2] > 100 && line_sensor[3] < 100)
			{
				shiftright(); // Shifts bot to the right slightly
			}
			
		}
		// Turns off both motors
		turn_off_right_motor();
		turn_off_left_motor();

		while (minesFound!=minecount) 
		{
				// Executes if sensor 0 is dark while 4 is light
				if (line_sensor[0] > 100 && line_sensor[4] < 100)
				{
					// Preps for turn twice
					TurnPrep(get_left_quadrature_counter());
					TurnPrep(get_left_quadrature_counter());
					rightTurn(get_left_quadrature_counter()); // right turn
					loopdirection = 0;
					// Turns off both motors
					turn_off_left_motor();
					turn_off_right_motor();
					break; // Breaks out of current loop
				}
				// Executes if sensor 4 is dark while sensor 0 is light
				if (line_sensor[4] > 160 && line_sensor[0] < 120)
				{
					// Preps for turn twice
					TurnPrep(get_left_quadrature_counter());
					TurnPrep(get_left_quadrature_counter());
					leftTurn(get_left_quadrature_counter()); // left turn
					loopdirection = 1; // Reverses loop direction
					// Turns off both motors
					turn_off_left_motor();
					turn_off_right_motor();
					break; // Breaks out of current loop
				}
			
			//use a pwm to control the speed of the bot
			pwm_counter = pwm_counter + 1;
			if( pwm_counter >= PWM_TOP ){
				pwm_counter = 0; // Resets pwm counter to 0
			}
		
			if (pwm_counter < 12) {
				// Turns on both motors
				turn_on_left_motor();
				turn_on_right_motor();
				// Switches direction of both motors forward
				turn_right_motor_forward();
				turn_left_motor_forward();
			}
			else {
				// Turns off both motors
				turn_off_left_motor();
				turn_off_right_motor();
			}
			// Executes while one of the middle sensors senses light
			while(line_sensor[1] < 100 && line_sensor[2] > 100 && line_sensor[3] > 100)
			{
				shiftleft(); // Shifts bot to the left slightly
			}
			// Executes while sensor 1 is sensning light
			while(line_sensor[1] > 100 && line_sensor[2] > 100 && line_sensor[3] < 100)
			{
				shiftright(); // Shifts  bot to the right slightly
			}
			_delay_us(10); // Delay for 10 microseconds
		
		} 
		
		while(minesFound != minecount)
		{
			
			//use a pwm to control the speed of the bot
			pwm_counter = pwm_counter + 1;
			if( pwm_counter >= PWM_TOP ){
				pwm_counter = 0; // Resets pwm counter
			}
		
			if (pwm_counter < 12) {
				// Turns both motors forward
				turn_right_motor_forward();
				turn_left_motor_forward();
				// Turns on motors
				turn_on_left_motor();
				turn_on_right_motor();
			}
			else {
				// Turns off motors
				turn_off_left_motor();
				turn_off_right_motor();
			}
		
			// Executes if all sensors are sensing light surfaces
			if (line_sensor[0] < 100 && line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100 && line_sensor[4] < 100)
			{
				minesFound++; // Increment nummber of mines found
				fullSpin(get_left_quadrature_counter()); // 360 spin
				// Turns off motors
				turn_off_left_motor();
				turn_off_right_motor();
				dispMineCount(minesFound); // Displays number of mines found
				TurnAround(get_left_quadrature_counter()); // 180 spin
				if(minesFound == minecount){
					break; // breaks of current loop
				}
				while(1)
				{
					// Turns both motors forward
					turn_right_motor_forward();
					turn_left_motor_forward();
					pwm_counter = pwm_counter + 1;
					if( pwm_counter >= PWM_TOP ){
						pwm_counter = 0; // Resets pwm counter
					}
					
					if (pwm_counter < 12) {
						// Turns on motors
						turn_on_left_motor();
						turn_on_right_motor();
					}
					else {
						// Turns off motors
						turn_off_left_motor();
						turn_off_right_motor();
					}
					if (line_sensor[0] > 100 && line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100 && line_sensor[4] > 100)
					{
						TurnAround(get_left_quadrature_counter()); // 180 spin
						break; // breaks out of current loop
					}
					
					// Executes while sensor 1 is detecting light off the dark line
					while(line_sensor[1] < 100 && line_sensor[2] > 100 && line_sensor[3] > 100)
					{
						shiftleft(); // Shifts bot to the left slightly
					}
					// Executes while sensor 3 is detecting ligt of the dark light
					while(line_sensor[1] > 100 && line_sensor[2] > 100 && line_sensor[3] < 100)
					{
						shiftright(); // Shifts bot to the right slightly
					}
				}
			}
		
			// Executes if sensors 1, 2, and 3 are detaacting light
			if (line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100)
			{
				TurnPrep(get_left_quadrature_counter()); // preps for a turn

				if(loopdirection == 0)
				{
					leftTurn(get_left_quadrature_counter()); // Left turn
					
				}
				else if(loopdirection == 1)
				{
					rightTurn(get_left_quadrature_counter()); // right turn
					
				}
				// Executes if sensor 1, 2, and 3 are detecting light
				if (line_sensor[1] < 100 && line_sensor[2] < 100 && line_sensor[3] < 100)
				{
					TurnAround(get_left_quadrature_counter()); // 180 spin
					loopdirection ^= 1;
					_delay_ms(500); // Delay of 500 milliseconds
					continue; // returns to beginning of current loop
				}
				else
				{
					break; // breaks out of current loop
				}
			}

			while(line_sensor[1] < 100 && line_sensor[2] > 100 && line_sensor[3] > 100)
			{
				shiftleft(); // Shifts bot to the left slightly
			}
			while(line_sensor[1] > 100 && line_sensor[2] > 100 && line_sensor[3] < 100)
			{
				shiftright(); // Shifts bot to the right slightly
			}
			
		}
	}
	
	celebration();// celebrate finding all mines
	
	//ensure the motors are turned off
	turn_off_left_motor();
	turn_off_right_motor();
}
