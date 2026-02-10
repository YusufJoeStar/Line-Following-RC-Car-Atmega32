#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

/* ===================== DEFINITIONS ===================== */
#define MAX_PWM 500   // PWM TOP value

/* --- Motor Pins (L298N) --- */
#define L_IN1 PB0
#define L_IN2 PB1
#define R_IN3 PB2
#define R_IN4 PB3

/* --- PWM Pins --- */
#define PWM_L PD5   // OC1A
#define PWM_R PD4   // OC1B

/* --- Sensors --- */
#define S_LEFT   PA0
#define S_CENTER PA1
#define S_RIGHT  PA2

/* --- LED --- */
#define LED_PIN PC0

/* ===================== PWM INIT ===================== */
void PWM_Init(void)
{
	// Fast PWM mode with ICR1 as TOP
	TCCR1A = (1<<COM1A1) | (1<<COM1B1) | (1<<WGM11);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11); // Prescaler 8
	ICR1 = MAX_PWM;
	
	// Set PWM pins as output
	DDRD |= (1<<PWM_L) | (1<<PWM_R);
	
	// Initialize with 0 duty cycle
	OCR1A = 0;
	OCR1B = 0;
}

/* ===================== MOTOR CONTROL ===================== */
void motors_stop(void)
{
	// Set all direction pins LOW
	PORTB &= ~((1<<L_IN1) | (1<<L_IN2) | (1<<R_IN3) | (1<<R_IN4));
	// Set PWM to 0
	OCR1A = 0;
	OCR1B = 0;
}

void forward(void)
{
	// Left motor forward
	PORTB |=  (1<<L_IN1);
	PORTB &= ~(1<<L_IN2);
	
	// Right motor forward
	PORTB |=  (1<<R_IN3);
	PORTB &= ~(1<<R_IN4);
	
	
	OCR1A = 200;  // Left motor
	OCR1B = 200;  // Right motor
}

void turn_left(void)
{
	// Left motor SLOW forward
	PORTB |=  (1<<L_IN1);
	PORTB &= ~(1<<L_IN2);
	
	// Right motor FAST forward
	PORTB |=  (1<<R_IN3);
	PORTB &= ~(1<<R_IN4);
	
	
	OCR1A = 100;  
	OCR1B = 250;  
}

void turn_right(void)
{
	// Left motor FAST forward
	PORTB |=  (1<<L_IN1);
	PORTB &= ~(1<<L_IN2);
	
	// Right motor SLOW forward
	PORTB |=  (1<<R_IN3);
	PORTB &= ~(1<<R_IN4);
	
	
	OCR1A = 250;  
	OCR1B = 100;  
}

/* ===================== MAIN ===================== */
int main(void)
{
	
	DDRB |= (1<<L_IN1) | (1<<L_IN2) | (1<<R_IN3) | (1<<R_IN4);
	
	
	DDRC |= (1<<LED_PIN);
	
	/* Sensors as input with pullups */
	DDRA &= ~((1<<S_LEFT) | (1<<S_CENTER) | (1<<S_RIGHT));
	PORTA |= (1<<S_LEFT) | (1<<S_CENTER) | (1<<S_RIGHT);
	
	/* Initialize PWM */
	PWM_Init();
	
	
	motors_stop();
	
	
	for (uint8_t i = 0; i < 3; i++)
	{
		PORTC |= (1<<LED_PIN);
		_delay_ms(200);
		PORTC &= ~(1<<LED_PIN);
		_delay_ms(200);
	}
	
	
	PORTC |= (1<<LED_PIN);
	
	/* Small delay before starting */
	_delay_ms(500);
	
	while (1)
	{
		
		uint8_t L = (PINA & (1<<S_LEFT));   
		uint8_t C = (PINA & (1<<S_CENTER)); 
		uint8_t R = (PINA & (1<<S_RIGHT)); 
		
		// Line following logic
		if (C && !L && !R)
		{
			// Only center sees black - go straight
			forward();
		}
		else if (L && !R)
		{
			// Left sensor sees black - STEEP LEFT TURN
			turn_left();
		}
		else if (R && !L)
		{
			// Right sensor sees black - STEEP RIGHT TURN
			turn_right();
		}
		else if (!L && !C && !R)
		{
			// All sensors see white - keep turning right to find the black line
			turn_right();
		}
		else if (L && C && R)
		{
			// All sensors see black - might be at intersection, go forward
			forward();
		}
		else
		{
			// Default: continue forward
			forward();
		}
		
		_delay_ms(20);  // Small delay for stability
	}
}
