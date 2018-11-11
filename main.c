/* Name: main.c
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h"
#include <math.h>


char buf[26];// volatile since interrupt may change value                  
volatile unsigned char pos;
volatile char process_packet;

int x =0;
int x1 =5;
int y =0;
int y1 =5;
int z =0;


int freq, prev_val;
unsigned int count, count2, prev, current;
int motor1Number=5;
int motor2Number=5;
int servoNumber=1;
int prevnumber=1;


ISR(SPI_STC_vect)

{ // grab byte from SPI Data Register     

unsigned char c= SPDR; 
SPDR = 0x7E;
if(pos < sizeof buf) { // add to buffer if room
buf [pos++] = c;
if(c == '\n')   // set flag if newline
process_packet = 1;
}
 else pos = 0;
}  // end of interrupt routine SPI_STC_vect 








int main(void)
{
	
	int i;
	m_usb_init();
	set(DDRB,3); // set  MISO B3 as output, 

	set(SPCR, SPE);  // enable SPI                       
	set(SPCR, SPIE); // enable SPI interrupt                   
	sei();  // enable all interrupts   


	teensy_clockdivide(0);	//set system clock to 16 MHz
	teensy_disableJTAG();	//when using F4-7, must disable JTAG
	
	//Set inputs & outputs
	
	set(DDRD, 7);		//motor 1 PWM
	set(DDRB, 4);		//motor 1 In1
	set(DDRB, 5);		//motor 1 In2
	
	set(DDRD, 0);		//motor 2 PWM
	set(DDRF, 0);		//motor 2 In1
	set(DDRF, 1);		//motor 2 In2
	
	set(DDRB, 6);		//servo pin output
	clear(DDRC, 7);		//"hit" button input pin
	clear(DDRF, 4);		//"hit" button input pin
	clear(DDRF, 5);
	
	clear(DDRF, 7);		//healing sensor pin input
	clear(PORTF, 7);
	
	set(DDRD, 6);		//onboard LED pin
	set(DDRF, 6);		//white
	clear(PORTF,6);
	//	clear(PORTD,6);
	
	//__Timer 4__Motor1
		
	clear(TCCR4B, CS43);
	clear(TCCR4B, CS42);
	clear(TCCR4B, CS41);	//turn on clock source 16MHz
	set(TCCR4B, CS40);
			
	clear(TCCR4D, WGM41);	// UP mode (8 bit = 255) to OCR4C
	clear(TCCR4D, WGM40);
			
	set(TCCR4C, PWM4D);
	set(TCCR4C, COM4D1);	//clear D7 at OCR4D, set at rollover (255)
	clear(TCCR4C, COM4D0);
	
	OCR4C = 255;	//set top value for Timer 4
		
	//__Timer 0__Motor2
	
	clear(TCCR0B, CS02);	//turn on clock source 16MHz
	clear(TCCR0B, CS01);
	set(TCCR0B, CS00);
		
	clear(TCCR0B, WGM02);		//UP to 255
	set(TCCR0A, WGM01);
	set(TCCR0A, WGM00);
	
	clear(TCCR0A, COM0B0);	//clear D0 at OCR0B, set at rollover (255)
	set(TCCR0A, COM0B1);
	
	//__Timer 3__Frequency
		
	clear(TCCR3B, CS30);
	clear(TCCR3B, CS31);
	set(TCCR3B, CS32);		//turn on clock source 16MHz/256 = 62500
	
	clear(TCCR3B, WGM33);	//mode 4, up to OCR3A
	set(TCCR3B, WGM32);
	clear(TCCR3A, WGM31);
	clear(TCCR3A, WGM30);
	
	OCR3A = 62500;	//set max counter value, so 1 count = 1 Hz
	
	clear(TCCR3A, COM3A0);	//no change to pin C6
	clear(TCCR3A, COM3A1);
	
	//__Timer 1__Servo
	
	clear(TCCR1B, CS12);	//turn on clock source 2MHz
	set(TCCR1B, CS11);
	clear(TCCR1B, CS10);	
	
	set(TCCR1B, WGM13);	//mode 15, UP to OCR1A (16-bit = 65536)
	set(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	set(TCCR1A, WGM10);
	
	OCR1A = 40000;	//40,000/2,000,000 = 20ms per cycle
	
	clear(TCCR1A, COM1B0);	//clear B6 at OCR1B, set at rollover (255)
	set(TCCR1A, COM1B1);
		
// 		m_usb_init();	//initialize the USB subsystem
// 		while(!m_usb_isconnected());	//wait for connection
	 
for(;;){
	update_data();

//motor1
if (motor1Number > 5){
	
	set(PORTB, 4);		//forward direction
	clear(PORTB, 5);

	switch(motor1Number) {
		case 10:
			OCR4D = 255;	//full speed
			break;
		case 9:
			OCR4D = 200;
			break;
		case 8:
			OCR4D = 150;
			break;
		case 7:
			OCR4D = 100;
			break;
		case 6:
			OCR4D = 50;	
			break;
	}
}

else if (motor1Number < 5){

	clear(PORTB, 4);		//backward direction
	set(PORTB, 5);

	switch(motor1Number) {
		case 4:
			OCR4D = 50;
			break;
		case 3:
			OCR4D = 100;
			break;
		case 2:
			OCR4D = 150;
			break;
		case 1:
			OCR4D = 200;
			break;
		case 0:
			OCR4D = 255;	//full speed
			break;
	}
}

else if (motor1Number == 5){
	OCR4D = 0;			//stop PWM
	set(PORTB, 4);		//brake motors
	set(PORTB, 5);
}

//motor2
if (motor2Number > 5){
	
	set(PORTF, 0);		//forward direction
	clear(PORTF, 1);

	switch(motor2Number) {
		case 10:
			OCR0B = 255;	//full speed
			break;
		case 9:
			OCR0B = 200;
			break;
		case 8:
			OCR0B = 150;
			break;
		case 7:
			OCR0B = 100;
			break;
		case 6:
			OCR0B = 50;
			break;
	}
}

else if (motor2Number < 5){

	clear(PORTF, 0);		//backward direction
	set(PORTF, 1);

	switch(motor2Number) {
		case 4:
			OCR0B = 50;
			break;
		case 3:
			OCR0B = 100;
			break;
		case 2:
			OCR0B = 150;
			break;
		case 1:
			OCR0B = 200;
			break;
		case 0:
			OCR0B = 255;	//full speed
			break;
	}
}

else if (motor2Number == 5){
	OCR0B = 0;			//stop PWM
	set(PORTF, 0);		//motor brake
	set(PORTF, 1);
}
	

//Servo Control

if(servoNumber!=prevnumber)
{
switch(servoNumber){
	case 1:
		OCR1B = 3900;	//servo UP position
		m_usb_tx_string("weapon up ");
		//moved=1;
		prevnumber=servoNumber;
		break;
	case 2:
		OCR1B = 2500;	//servo DOWN position
		m_usb_tx_string("weapon down ");
		//moved=1;
		prevnumber=servoNumber;
		break;
		//servo
		//min 1100 (~.55ms pulse), max 4900 (~2.45 ms pulse)
	
}
}
		
//calculate frequency of healing LED
if(bit_is_clear(PINF, 7)){
	count = count + 1;	//increment count
	if (count2 > 0){
		current = TCNT3;	//set counter value to current
		count2 = 0;			//reset count2
				
		freq = 1/(2*.000016*(current - prev));	//calculate frequency using difference in counter value and 1/clock frequency
				
		/*m_usb_tx_string("frequency = ");	//print statements
		m_usb_tx_uint(freq);
		m_usb_tx_string("\n");
		*/		
		TCNT3 = 0;		//reset counter value
	}
}
if(bit_is_set(PINF, 7)){
	count2 = count2 + 1;	//increment count2
	if (count > 0){
		prev = TCNT3;	//set counter value to prev
		count = 0;		//reset count
	}
}

if(freq>200 && freq<400){
	set(PORTF,6);	//turn on onboard LED
}
		
else
{
	clear(PORTF,6);
}


if (bit_is_set(PINC,7)||bit_is_set(PINF,4)||bit_is_set(PINF,5))
{
	set(PORTD,6);	//turn on onboard LED
}else
{
	clear(PORTD,6);
}



}
    return 0;   /* never reached */
}



void update_data() {
	
if(process_packet) {
if(pos<7&&pos>5)	
{
x = buf[0] - '0';
x1 = buf[1] - '0';
y = buf[2] - '0';
y1 = buf[3] - '0';
servoNumber = buf[4] - '0';
motor1Number=(x*10)+x1;

motor2Number=(y*10)+y1;
m_usb_tx_int(motor1Number);
m_usb_tx_char(10);
m_usb_tx_char(13);
m_usb_tx_int(motor2Number);
m_usb_tx_char(10);
m_usb_tx_char(13);
m_usb_tx_int(servoNumber);
m_usb_tx_char(10);
m_usb_tx_char(13);
}

pos = 0;
process_packet = 0;
 }  // end of process_packet
}  // end of loop