/* ===================== SYSTEM CONFIGURATION ===================== */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* ===================== DEFINITIONS ===================== */
#define SENSOR_COUNT 5
#define MAX_PWM 1000
#define LOST_LINE_THRESHOLD 0  // Now: if all sensors read 0 (no line detected)
#define RUN_STATE 1
#define STOP_STATE 0

/* --- Motor Pins (L298N) --- */
#define L_IN1 PB0
#define L_IN2 PB1
#define R_IN3 PB2
#define R_IN4 PB3

/* --- LED Indicator --- */
#define LED_PIN PC0

/* --- Start/Stop Button (INT0) --- */
#define BUTTON_PIN PD2

/* --- Digital IR Sensor Pins (PORTA) --- */
#define SENSOR_PORT PINA
#define SENSOR_PIN0 PA0
#define SENSOR_PIN1 PA1
#define SENSOR_PIN2 PA2
#define SENSOR_PIN3 PA3
#define SENSOR_PIN4 PA4

/* ===================== FUNCTION PROTOTYPES ===================== */
void Sensors_Init(void);
uint8_t Sensor_Read(uint8_t sensorIndex);
void PWM_Init_Timer1(void);
void Motor_Set(int16_t left, int16_t right);
void Motor_SetDirection(int16_t left, int16_t right);
void Timer0_Init_CTC(void);
void LineFollow_Control(void);
void System_InitPins(void);

/* ===================== GLOBAL VARIABLES ===================== */
volatile uint8_t controlFlag = 0;
volatile uint8_t systemState = STOP_STATE;
float Kp = 50.0;

/* Additional safety variables */
float previousError = 0;
uint8_t sensorFault = 0;  // Detect sensor reading issues
uint8_t lastSensorValues[SENSOR_COUNT] = {0};  // For noise rejection / filtering
uint8_t sensorStableCount = 0;  // Debounce counter for sensor readings

/* ===================== DIGITAL SENSOR FUNCTIONS ===================== */
void Sensors_Init(void) {
	/* Configure PORTA pins 0-4 as inputs for IR sensors */
	DDRA &= ~((1 << SENSOR_PIN0) | (1 << SENSOR_PIN1) | (1 << SENSOR_PIN2) |
	(1 << SENSOR_PIN3) | (1 << SENSOR_PIN4));
	
	/* Enable internal pull-ups for sensors (active LOW configuration) */
	/* This means: Line detected = LOW (0), No line = HIGH (1) */
	PORTA |= ((1 << SENSOR_PIN0) | (1 << SENSOR_PIN1) | (1 << SENSOR_PIN2) |
	(1 << SENSOR_PIN3) | (1 << SENSOR_PIN4));
}

uint8_t Sensor_Read(uint8_t sensorIndex) {
	/* Bound check */
	if (sensorIndex >= SENSOR_COUNT) {
		sensorFault = 1;
		return lastSensorValues[sensorIndex];
	}
	
	/* Read the sensor pin and invert logic */
	/* IR sensors typically output LOW when line is detected */
	/* We return 1 when line is detected, 0 when no line */
	uint8_t pinValue = (SENSOR_PORT >> sensorIndex) & 0x01;
	uint8_t sensorValue = !pinValue;  // Invert: LOW (line) -> 1, HIGH (no line) -> 0
	
	/* Store for fault recovery */
	lastSensorValues[sensorIndex] = sensorValue;
	
	return sensorValue;
}

/* ===================== MOTOR CONTROL ===================== */
void PWM_Init_Timer1(void) {
	/* Fast PWM mode with ICR1 as TOP */
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // Prescaler = 8
	ICR1 = MAX_PWM;
	
	/* Set PWM pins as outputs (PD4 = OC1B, PD5 = OC1A) */
	DDRD |= (1 << PD5) | (1 << PD4);
}

void Motor_SetDirection(int16_t left, int16_t right) {
	/* Left Motor */
	if (left >= 0) {
		PORTB |= (1 << L_IN1);
		PORTB &= ~(1 << L_IN2);
		} else {
		PORTB &= ~(1 << L_IN1);
		PORTB |= (1 << L_IN2);
	}
	
	/* Right Motor */
	if (right >= 0) {
		PORTB |= (1 << R_IN3);
		PORTB &= ~(1 << R_IN4);
		} else {
		PORTB &= ~(1 << R_IN3);
		PORTB |= (1 << R_IN4);
	}
}

void Motor_Set(int16_t left, int16_t right) {
	Motor_SetDirection(left, right);
	
	/* Absolute values */
	if (left < 0) left = -left;
	if (right < 0) right = -right;
	
	/* Clamp values */
	if (left > MAX_PWM) left = MAX_PWM;
	if (right > MAX_PWM) right = MAX_PWM;
	
	OCR1A = left;
	OCR1B = right;
}

/* ===================== TIMER0 INTERRUPT ===================== */
void Timer0_Init_CTC(void) {
	/* CTC mode, prescaler = 64 */
	TCCR0 = (1 << WGM01) | (1 << CS01) | (1 << CS00);
	OCR0 = 124;  // ~20ms interrupt at 16MHz
	TIMSK |= (1 << OCIE0);
}

ISR(TIMER0_COMP_vect) {
	controlFlag = 1;
}

/* ===================== START/STOP BUTTON ===================== */
ISR(INT0_vect) {
	/* Simple debouncing with delay */
	_delay_ms(50);
	
	/* Check if button is still pressed (LOW due to pull-up) */
	if (!(PIND & (1 << BUTTON_PIN))) {
		systemState ^= 1;  // Toggle state
	}
}

/* ===================== CONTROL LOOP ===================== */
void LineFollow_Control(void) {
	uint8_t sensorValues[SENSOR_COUNT];
	uint8_t sum = 0;
	int16_t weighted = 0;
	const int8_t weight[SENSOR_COUNT] = {-2, -1, 0, 1, 2};
	
	/* Read all sensors */
	for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
		sensorValues[i] = Sensor_Read(i);
		sum += sensorValues[i];
		weighted += (int16_t)sensorValues[i] * weight[i];
	}
	
	/* Safety fall-back: Sensor failure mode */
	if (sensorFault) {
		Motor_Set(0, 0);
		sensorFault = 0;  // Reset fault flag
		return;
	}
	
	/* Lost line condition - no sensors detect the line */
	if (sum == LOST_LINE_THRESHOLD) {
		/* Rotate in place to search for line */
		/* Use previous error to determine rotation direction */
		if (previousError > 0) {
			Motor_Set(250, -250);  // Turn right
			} else {
			Motor_Set(-250, 250);  // Turn left
		}
		return;
	}
	
	/* Calculate position error */
	/* Error ranges from -2 to +2 based on line position */
	float error = (float)weighted / (float)sum;
	
	/* P control correction */
	float correction = Kp * error;
	
	/* Base speed and differential steering */
	int16_t base = 650;
	int16_t leftSpeed = base + correction;
	int16_t rightSpeed = base - correction;
	
	/* Apply motor speeds */
	Motor_Set(leftSpeed, rightSpeed);
	
	/* Store error for lost line recovery */
	previousError = error;
}

/* ===================== PIN INITIALIZATION ===================== */
void System_InitPins(void) {
	/* Motor control pins as outputs */
	DDRB |= (1 << L_IN1) | (1 << L_IN2) | (1 << R_IN3) | (1 << R_IN4);
	
	/* LED pin as output */
	DDRC |= (1 << LED_PIN);
	
	/* Button pin as input with pull-up */
	DDRD &= ~(1 << BUTTON_PIN);
	PORTD |= (1 << BUTTON_PIN);  // Enable internal pull-up
	
	/* Configure INT0 for falling edge (button press) */
	MCUCR |= (1 << ISC01);  // Falling edge trigger
	MCUCR &= ~(1 << ISC00);
	GICR |= (1 << INT0);  // Enable INT0
}

/* ===================== MAIN ===================== */
int main(void) {
	System_InitPins();
	Sensors_Init();  // Initialize digital IR sensors
	PWM_Init_Timer1();
	Timer0_Init_CTC();
	
	sei();  // Enable global interrupts
	
	while (1) {
		/* STOP state - motors off, LED off */
		if (systemState == STOP_STATE) {
			Motor_Set(0, 0);
			PORTC &= ~(1 << LED_PIN);
			continue;
		}
		
		/* RUN state - LED on, control active */
		PORTC |= (1 << LED_PIN);
		
		if (controlFlag) {
			controlFlag = 0;
			LineFollow_Control();
		}
	}
}