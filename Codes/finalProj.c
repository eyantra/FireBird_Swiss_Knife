#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdio.h>
#include<string.h>
#define WHEEL_DIST 15 // in cm
#include <math.h> //included to support power function
#include "lcd.c"


#define FCPU 11059200ul //defined here to make sure that program works properly
#define DIST_PER_TICK 0.544
#define TICKS_PER_SEC 11059

void (*timer3func)(void);
void (*timer2func)(void);
void (*timer1func)(void);
void port_init();
void timer5_init();
void velocity(unsigned char, unsigned char);
void motors_delay();
int _rollLCDFlag,_LCDpos;
unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char flag1 = 0;
unsigned char flag2 = 0;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;
unsigned char Front_Sharp_Sensor=0;
unsigned char Front_IR_Sensor=0;


unsigned long long int curLeftCounter,curRightCounter,splLeftCounter,splRightCounter,functionFlag=0;
unsigned char leftVel,rightVel;
float botRatio,botAngle,botDistance;
unsigned char data;
char _rollLCD[100];
unsigned int leftInt=0,rightInt;

//! A LCD-initialization function.
    /*!
	  the function to initialize use of LCD on the Bot
    */
//Function to configure LCD port
void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}
//! A ADC-configuration function.
    /*!
	  the function to configure use of ADC on the Bot
    */
//ADC pin configuration
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}
//! A motion-configuration function.
    /*!
	  the function to configure use of motion on the Bot
    */
//Function to configure ports to enable robot's motion
void motion_pin_config (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}
//! A buzzer-configuration function.
    /*!
	  the function to configure use of buzzer on the Bot
    */
//Function to configure Buzzer pins
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

//! A basic-initialization function.
    /*!
	  the function to initialize ports for use of basic functionalities on the Bot
    */
//Function to Initialize PORTS
void port_init()
{
	lcd_port_config();
	adc_pin_config();
	motion_pin_config();
	left_encoder_pin_config();
	right_encoder_pin_config();
	buzzer_pin_config();	
}

//! A timer5-initialization function.
    /*!
	  the function to initialize use of  timer5 on the Bot
    */
// Timer 5 initialised in PWM mode for velocity control
// Prescale:64
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:674.988Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionalit to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

//! A Buzzer on function.
    /*!
	  the function to switch the buzzer on
    */
void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

//! A Buzzer off function.
    /*!
	  the function to switch the buzzer off
    */
void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}
//! A adc initialization function
    /*!
	  the function to initialize ports for adc
    */
void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//! Function For ADC Conversion
    /*!
	  Function For ADC Conversion port setting
    */

unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}
//! Print sensor values on LCD
    /*!
	 Function To Print Sesor Values At Desired Row And Coloumn Location on LCD
    */

void print_sensor(char row, char coloumn,unsigned char channel)
{
	
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}
//! velocity control
    /*!
	 Function for setting velocities of both wheels. pulse width modulation
    */

void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
leftVel=left_motor;
	rightVel=right_motor;
}
//! setting motor's direction
    /*!
	 Function used for setting motor's direction
    */

void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 		// removing upper nibbel for the protection
 PortARestore = PORTA; 		// reading the PORTA original status
 PortARestore &= 0xF0; 		// making lower direction nibbel to 0
 PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
 PORTA = PortARestore; 		// executing the command
}

//! setting motor's forward
    /*!
	 Function used for setting motor's direction forward
    */
void forward (void) 
{
  motion_set (0x06);
}

//! setting motor's direction back
    /*!
	 Function used for setting motor's direction back
    */
void back (void) //both wheels backward
{
  motion_set(0x09);
}

//! setting motor's direction left
    /*!
	 Function used for setting motor's direction left
    */
void left (void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

//! setting motor's direction right
    /*!
	 Function used for setting motor's direction right
    */
void right (void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

//! setting motor's direction soft left
    /*!
	 Function used for setting motor's direction soft left
    */
void soft_left (void) //Left wheel stationary, Right wheel forward
{
 motion_set(0x04);
}

//! setting motor's direction soft right
    /*!
	 Function used for setting motor's direction soft right 
	 Left wheel forward, Right wheel is stationary
    */
void soft_right (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

//! setting motor's direction soft left
    /*!
	 Function used for setting motor's direction soft left
	 Left wheel backward, right wheel stationary
    */
void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
 motion_set(0x01);
}

//! setting motor's direction soft right
    /*!
	 Function used for setting motor's direction
	 Left wheel stationary, Right wheel backward
    */
void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
 motion_set(0x08);
}

//! stop the bot
    /*!
	 Function used for stoping the bot.
    */
void stop (void)
{
  motion_set (0x00);
}



////interrupt vectore...............
//! starting the left wheel encoder
    /*!
	 Function used for starting the left wheel encoder
    */
void left_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pullup for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
//! starting the right wheel encoder
    /*!
	 Function used for starting the right wheel encoder
    */
void right_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pullup for PORTE 4 pin
}
//Function to initialize ports

//! enabling the left wheel encoder
    /*!
	 Function used for enabling the left wheel encoder
    */
void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt 
}

//! enabling the right wheel encoder
    /*!
	 Function used for enabling the right wheel encoder
    */
void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
 EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
 sei();   // Enables the global interrupt 
}


//ISR for right position encoder
//! Interrupt 5 - right wheel
    /*!
	 ISR for right position encoder
    */
ISR(INT5_vect)  
{
	rightInt++;
	curRightCounter++;  //increment right shaft position count

}


//SR for left position encoder
//! Interrupt 4 - left wheel
    /*!
	 ISR for left position encoder
    */
ISR(INT4_vect)
{
	leftInt++;
	curLeftCounter++;  //increment left shaft position count

}

/////////////////interrupt vectors

////zigbee
//!zigbee usage    
	/*!
	 Function used for initializing zigbee usage
    */
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x47; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}
//! internal struct to recieve data from zigbee
	/*! internal struct used to recieve data from zigbee. uses linked list structure
	*/
struct charpack
{
    char data;
    struct charpack *next;
} typedef charPack;
charPack *serialDataStart=NULL;
charPack *serialDataEnd=NULL;
int serial_lock=0;
 
 
 //! zigbee data recieve
    /*!
	 intterupt generated when data is recieved from zigbee
    */
SIGNAL(SIG_USART0_RECV)         // ISR for receive complete interrupt
{
    data = UDR0;                 //making copy of data from UDR0 in 'data' variable 
  
    charPack *temp;

    temp=(charPack *) malloc(sizeof(charPack));
    temp->next=NULL;
    temp->data=(char) data;
    serial_lock=1;
       //lcd_wr_char(temp->data);
    if(serialDataEnd==NULL) serialDataEnd=temp;
    else
    {
         serialDataEnd->next=temp;
         serialDataEnd=temp;
    }
    if(serialDataStart==NULL) serialDataStart=serialDataEnd;     

}

//! zigbee sendString
    /*!
	 function to recieve a string from zigbee followed by a marker terminal #
    */
void serial_sendString(char str[])
{int i;
lcd_cursor(1,1);
UDR0='#';
_delay_ms(1);
char val[20];
//lcd_string("here");
//itoa(strlen(str),val,10);
//	lcd_string(val);

	for(i=0;i<strlen(str);i++)
	{
	UDR0=str[i];
//	lcd_wr_char(str[i]);

	_delay_ms(1);
	}
	UDR0='#';
	_delay_ms(1);
}

//! function to invoke actions based on input
    /*!
	 Function used for invoking action based on command recieved form the computer via zigbee
    */
void invoker(char opt[50])
{
	    if(!strcmp("w",opt)) //ASCII value of 8
        {
            PORTA=0x06;  //forward
			return;
        }
 
        if(!strcmp("s",opt)) //ASCII value of 2
        {
            PORTA=0x09; //back
			return;
        }
 
        if(!strcmp("a",opt)) //ASCII value of 4
        {
            PORTA=0x05;  //left
  	        return;
		}
 
        if(!strcmp("d",opt)) //ASCII value of 6
        {
            PORTA=0x0A; //right
			return;
        }
 
        if(!strcmp(" ",opt)) //ASCII value of 5
        {
            PORTA=0x00; //stop
			return;
        }
 
        if(!strcmp("h",opt)) //ASCII value of 7
        {
            buzzer_on();
			return;
        }
 
        if(!strcmp("m",opt)) //ASCII value of 9
        {
            buzzer_off();
			return;
        }
		if(!strcmp("lcd",opt))
		{
			char arg1[50];
			serial_getString(arg1,50);
			lcd_cursor(1,1);
			lcd_clear();
			lcd_string(arg1);
			return;
		}
		if(!strcmp("setPort",opt))
		{
			char portName[50],value[50];
				serial_getString(portName,50);serial_getString(value,50); 
				  if(strcmp(portName,"PORTA")==0) PORTA=atoi(value);
				  else if(strcmp(portName,"PORTB")==0) PORTB=atoi(value);
				  else if(strcmp(portName,"PORTC")==0) PORTC=atoi(value);
				  else if(strcmp(portName,"PORTD")==0) PORTD=atoi(value);
				  else if(strcmp(portName,"PORTE")==0) PORTE=atoi(value);
				  else if(strcmp(portName,"PORTF")==0) PORTF=atoi(value);
				  else if(strcmp(portName,"PORTG")==0) PORTG=atoi(value);
				  else if(strcmp(portName,"PORTH")==0) PORTH=atoi(value);
				  else if(strcmp(portName,"PORTJ")==0) PORTJ=atoi(value);
				  else if(strcmp(portName,"PORTK")==0) PORTK=atoi(value);
				  else if(strcmp(portName,"PORTL")==0) PORTL=atoi(value);
				  else if(strcmp(portName,"DDRA")==0) DDRA=atoi(value);
				  else if(strcmp(portName,"DDRB")==0) DDRB=atoi(value);
				  else if(strcmp(portName,"DDRC")==0) DDRC=atoi(value);
				  else if(strcmp(portName,"DDRD")==0) DDRD=atoi(value);
				  else if(strcmp(portName,"DDRE")==0) DDRE=atoi(value);
				  else if(strcmp(portName,"DDRF")==0) DDRF=atoi(value);
				  else if(strcmp(portName,"DDRG")==0) DDRG=atoi(value);
				  else if(strcmp(portName,"DDRH")==0) DDRG=atoi(value);
				  else if(strcmp(portName,"DDRJ")==0) DDRJ=atoi(value);
				  else if(strcmp(portName,"DDRK")==0) DDRK=atoi(value);
				  else if(strcmp(portName,"DDRL")==0) DDRL=atoi(value);
				return;

		
		
		}
		if(!strcmp("getPin",opt))
		{
			char pinName[10],value[10];
			serial_getString(pinName,10); 
			if(strcmp(pinName,"PINA")==0)   itoa(PINA, value,10 ); 
			  else if(strcmp(pinName,"PINB")==0)   itoa(PINB, value,10 ); 
			  else if(strcmp(pinName,"PINC")==0)   itoa(PINC, value,10 ); 
			  else if(strcmp(pinName,"PIND")==0)   itoa(PIND, value,10 ); 
			  else if(strcmp(pinName,"PINE")==0)   itoa(PINE, value,10 ); 
			  else if(strcmp(pinName,"PINF")==0)   itoa(PINF, value,10 ); 
			  else if(strcmp(pinName,"PING")==0)   itoa(PING, value,10 ); 
			  else if(strcmp(pinName,"PINH")==0)   itoa(PINH, value,10 ); 
			  else if(strcmp(pinName,"PINJ")==0)   itoa(PINJ, value,10 ); 
			  else if(strcmp(pinName,"PINK")==0)   itoa(PINK, value,10 ); 
			  else if(strcmp(pinName,"PINL")==0)   itoa(PINL, value,10 ); 
			serial_sendString(value);
			return;

		}
		if(!strcmp("rollLCD",opt))
		{

			char str[100];
			serial_getString(str,100);
			rollLCD(str);
			return;

		}
		if(!strcmp("strictForward",opt))
		{
			strictForward();return;
			
		}
		if(!strcmp("strictBack",opt))
		{
			strictBackward();return;
			
		}
		if(!strcmp("moveOnArc",opt))
		{

			char rad[20],dir[3];
			serial_getString(rad,20);
			serial_getString(dir,3);
			int radius,direc;
			radius=atoi(rad);
			direc=atoi(dir);
			moveOnArc(radius,direc);
			
			return;

		}
		if(!strcmp("getLeftWLS",opt))
		{
		char value[10];
		int a=ADC_Conversion(3);
		itoa(a, value,10 );
		serial_sendString(value);
		return;
		}
		if(!strcmp("getCenterWLS",opt))
		{
		char value[10];
		int a=ADC_Conversion(2);
		itoa(a, value,10 );
		serial_sendString(value);
		return;
		}
		if(!strcmp("getRightWLS",opt))
		{
		char value[10];
		int a=ADC_Conversion(1);
		itoa(a, value,10 );
		serial_sendString(value);
		return;
		}
		if(!strcmp("getIRProx",opt))
		{
			char sensorNo[4];
			serial_getString(sensorNo,4);
			int sen=atoi(sensorNo);
			if(sen>5||sen<1)
			{	lcd_string("wrong sensor no.");
				return;
			}
			char value[10];
			int a=ADC_Conversion(sen+3);
			itoa(a, value,10 );
			serial_sendString(value);
			return;
		}
		if(!strcmp("getIRSharp",opt))
		{
			char sensorNo[4];
			serial_getString(sensorNo,4);
			int sen=atoi(sensorNo);
			if(sen>5||sen<1)
			{	lcd_string("wrong sensor no.");
				return;
			}
			char value[10];
			int a=ADC_Conversion(sen+8);
			itoa(a, value,10 );
			serial_sendString(value);
			return;
		}
		if(!strcmp("setVelocity",opt))
		{
		
			char left[10],right[10];
			serial_getString(left,10);
			serial_getString(right,10);
			velocity(atoi(left)%255,atoi(right)%255);
			return;
		}
		if(!strcmp("listenForInterrupt",opt))
		{
			char intName[10],value[5];
			
			serial_getString(intName,10);
			if(!(strcmp(intName,"left")))
			{ 
				if(leftInt>0)
				{	
					itoa(leftInt, value, 5 );leftInt=0;
					serial_sendString(value);

				}
				else serial_sendString("0");
			}
			else if(!(strcmp(intName,"right")))
			{ 
				if(rightInt>0)
				{	
					itoa(rightInt, value, 5 );rightInt=0;
					serial_sendString(value);

				}
				else serial_sendString("0");
			}
			else
			{ 
				lcd_clear();
				lcd_string("wrong interrupt"); 
			}
			return;

		}
		if(!strcmp("resetInterrupt",opt))
		{
			char intName[10];
			
			serial_getString(intName,10);
			if(!(strcmp(intName,"left")))
			{ leftInt=0;
			}
			else if(!(strcmp(intName,"right")))
			{ rightInt=0;
			}
			else 
			{ 
				lcd_clear();
				lcd_string("wrong interrupt"); 
			}
			return;
		}


		lcd_string(opt);


}

 
 //! zigbee getString
    /*!
	 function to get a string from zigbee followed by a marker terminal #
    */
void serial_getString(char *x , int n)
{
    int i;
    charPack *runner=serialDataStart, *temp;
    for(i=0;i<n;i++)
    {   
	//	lcd_string("reading ");
		
        if(runner==NULL || serial_lock==1) {i=-1; serial_lock=0;runner=serialDataStart;continue;}
        else if(runner->data=='#') {
            x[i]='\0';
            runner=serialDataStart;
            while(runner->data!='#')
            { temp=runner;
              runner=runner->next;
              free(temp);       
            }
            serialDataStart=runner->next;
			if(serialDataStart==NULL) serialDataEnd=NULL;
            free(runner);
            return;                   
        }
        else
        {
            x[i]=runner->data;
			//lcd_wr_char(x[i]);
			runner=runner->next;			
        }
    }
     
}
/////////////zigbeeeeeeeeeeee
//! display a number on LCD
    /*!
	 Function used for displaying a number on LCD
    */
void lcd_num(int x)
{
char cVal[10];
itoa(x,cVal,10);
lcd_string(cVal);
char a=" ";
lcd_wr_char(a);
}
///timerrrrrrrrrrrrr


//! calculate turn ratio
    /*!
	 for traversing a arc/ a straight line, we have to calculate a urn ratio, that is the ratio of velocities of the left and right 
	 wheel velocities.
    */
float turnRatio(float radius,int dir)
{//radius is in cm

	int w;
	w=WHEEL_DIST;
	float ratio;
		ratio=(float) ((float)(radius+w/2.0))/((float) (radius-w/2.0));
		if(dir==2)//means left
		{	ratio=1.0/ratio;
		}
	return ratio;
}


//! strictForward
    /*!
	 Function used for setting motor's direction strictForward
    */
void strictForward()
{
botRatio=1.0;

functionFlag=1;
splRightCounter=curRightCounter;
splLeftCounter=curLeftCounter;
forward();

}

//! strictBackward
    /*!
	 Function used for setting motor's direction strictBackward
    */
void strictBackward()
{
botRatio=1.0;

functionFlag=1;
splRightCounter=curRightCounter;
splLeftCounter=curLeftCounter;
back();

}

//! rolling LCD display
    /*!
	 non-blocking rolling lcd display
    */
void rollLCD(char str[])
{
strcpy(_rollLCD,str);
strcat(_rollLCD,"               ");
lcd_clear();
lcd_cursor(1,1);
lcd_string(_rollLCD);
_LCDpos=1;
_rollLCDFlag=1;

}

//! setting motor's direction to move on arc
    /*!
	 Function used for setting motor's direction to move on arc of given radius.
	 dir=1 means left dir=0 means right
    */
void moveOnArc(float radius,int dir)
{

float ratio,newLeftVel,newRightVel;
ratio=turnRatio(radius,dir);

functionFlag=1;
botRatio=ratio;

leftVel=150;
rightVel=150;
	if(ratio>1)
	{
		newRightVel=rightVel;
		newLeftVel=(float) newRightVel*ratio;
		if(newLeftVel>254)
		{
			newLeftVel=leftVel;
			newRightVel=(float)newLeftVel/ratio;
		}
	}
	else 
	{
		newLeftVel=leftVel;
		newRightVel=(float) newLeftVel/ratio;
		if(newRightVel>254)
		{
			newRightVel=rightVel;
			newLeftVel=(float)newRightVel*ratio;
		}
	}
splLeftCounter=curLeftCounter;
splRightCounter=curRightCounter;

velocity((unsigned char) newLeftVel,(unsigned char) newRightVel);
forward();
}


//! setting timer 3
    /*!
	 Function used for setting timer 3 function for Oninterrupt usage
    */
void setTimer3(int time, void (*f) (void))
{
long long int ticks=TICKS_PER_SEC*time;
TCCR3B = 0x00; //stop
 TCNT3H = 0xFF - ticks/0xFF; //Counter higher 8 bit value
 TCNT3L = 0xFF-ticks%0xFF; //Counter lower 8 bit value
 TCCR3B = 0x05; //start Timer
 timer3func=f;

}

//! timer 3 overflow
    /*!
	 Interrupt on overflow of timer 3
    */
ISR(TIMER3_OVF_vect)
{

(*timer3func)();
}

//! setting timer 1
    /*!
	 Function used for setting timer 1 function for Oninterrupt usage
    */
void setTimer1(int time, void (*f) (void))
{
long long int ticks=TICKS_PER_SEC*time;
TCCR1B = 0x00; //stop
 TCNT1H = 0xFF - ticks/0xFF; //Counter higher 8 bit value
 TCNT1L = 0xFF-ticks%0xFF; //Counter lower 8 bit value
 TCCR1B = 0x05; //start Timer
 timer1func=f;

}

//! timer 1 overflow
    /*!
	 Interrupt on overflow of timer 1
    */
ISR(TIMER1_OVF_vect)
{

(*timer1func)();
}

//! setting motor's direction
    /*!
	 Function used for setting motor's direction
    */
ISR(TIMER4_OVF_vect)
{
//lcd_clear();
//lcd_string("timer");
if(functionFlag!=0)
{
//bot distance chk kr ke stop bhi karna hai-----------------------------------------


		
		int left,right;float cratio,velRatio;
	float dist;
		right=curRightCounter-splRightCounter;
		left=curLeftCounter-splLeftCounter;
	
		if(left!=0 && right!=0)
		{
			cratio=(float)left/right;
			velRatio = (float)leftVel/rightVel;
			int newLeftVel,newRightVel;
			if(cratio < botRatio - 0.001 )
			{
				float newVelRatio;
				newVelRatio =(float) ((float)botRatio/cratio)*velRatio;

				newRightVel=rightVel;
				newLeftVel=rightVel * newVelRatio;
				if(newLeftVel>254)
				{
				newLeftVel=leftVel;
				newRightVel=(float)leftVel/newVelRatio;
				}
				//lcd_clear();
	/*			
				lcd_cursor(1,1);
			//	lcd_string("H");
				lcd_num(left);
				lcd_num(right);

				lcd_num(leftVel);
				lcd_num(rightVel);
				lcd_cursor(2,2);
				lcd_num(newLeftVel);
				lcd_num(newRightVel);
				*/

				velocity((unsigned char) newLeftVel,(unsigned char) newRightVel);
				
			}
			else if( cratio > botRatio+0.001 )
			{
				float newVelRatio;
				newVelRatio = (botRatio/cratio)*velRatio;
				newLeftVel=leftVel;

				newRightVel=(float)leftVel/newVelRatio;
				if(newRightVel>254)
				{	
					newRightVel = rightVel;
					newLeftVel = newRightVel * newVelRatio;
				}
					//lcd_clear();
			/*	
				lcd_cursor(1,1);
			//	lcd_string("H");
				lcd_num(left);
				lcd_num(right);

				lcd_num(leftVel);
				lcd_num(rightVel);
				lcd_cursor(2,2);
				lcd_num(newLeftVel);
				lcd_num(newRightVel);
				*/
				velocity((unsigned char) newLeftVel,(unsigned char) newRightVel);
				
			}

		}
		if(functionFlag==1)
		{
			splRightCounter=curRightCounter;
			splLeftCounter=curLeftCounter;
		}
	
	
}
if(_rollLCDFlag==1)
{lcd_cursor(1,1);
	lcd_string(_rollLCD+_LCDpos);
	_LCDpos++;
		if(_LCDpos==strlen(_rollLCD) -14)
		{ 
		_rollLCDFlag=0;
		lcd_clear();
	
		}
}
 //TIMER4 has overflowed
 TCNT4H = 0xEF; //reload counter high value
 TCNT4L = 0x00; //reload counter low value
//lcd_clear();
} 

//! timer 4 initialization
    /*!
	 timer 4 initialization and setting high and low values and enable overflow interrupt
    */
void timer4_init(void)
{
 TCCR4B = 0x00; //stop
 TCNT4H = 0xEF; //Counter higher 8 bit value
 TCNT4L = 0x00; //Counter lower 8 bit value
 OCR4AH = 0x00; //Output Compair Register (OCR)- Not used
 OCR4AL = 0x00; //Output Compair Register (OCR)- Not used
 OCR4BH = 0x00; //Output Compair Register (OCR)- Not used
 OCR4BL = 0x00; //Output Compair Register (OCR)- Not used
 OCR4CH = 0x00; //Output Compair Register (OCR)- Not used
 OCR4CL = 0x00; //Output Compair Register (OCR)- Not used
 ICR4H  = 0x00; //Input Capture Register (ICR)- Not used
 ICR4L  = 0x00; //Input Capture Register (ICR)- Not used
 TCCR4A = 0x00; 
 TCCR4C = 0x00;
 TCCR4B = 0x05; //start Timer
}
int c=0;





////timerrrrrrrrrrrrrrrrrr 44444444

//! initialize all devices
    /*!
	 initialize all devices. call all other initialization function
    */
void init_devices (void)
{
 	cli(); //Clears the global interrupts
	port_init();
	adc_init();
	uart0_init();
	timer5_init();
	timer4_init();

 TIMSK4 = 0x01; //timer4 overflow interrupt enable
 TIMSK3 = 0x01;
 left_position_encoder_interrupt_init();
 right_position_encoder_interrupt_init();
 button_interrupt_init();
	sei();   //Enables the global interrupts
}

int a=0;
//! a example function
    /*!
	 a example function for use onInterrupt
    */
void test()
{a++;
	lcd_clear();
	lcd_cursor(1,1);
lcd_num(a);
	lcd_string("hello world");
	setTimer3(3,&test);
}

int main()
{
	init_devices();
	lcd_set_4bit();
	lcd_init();
	leftVel=100;
	rightVel=100;
	functionFlag=0;

velocity(254,254);

while(1)
{
	char a[100];
	serial_getString(a,100);
	invoker(a);
}

	
}

