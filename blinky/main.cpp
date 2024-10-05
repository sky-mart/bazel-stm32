#include "stm32f3xx.h"

GPIO_TypeDef * const port = GPIOE;
const int led = 13;

void softDelay(volatile uint32_t time)
{
	while (time > 0) {
		time--;
	}
}

int main()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN; // Clock

	port->MODER 	|= 1 << led*2; 	// General purpose output mode
	port->OTYPER 	&= ~(1 << led); // Push-pull
	port->OSPEEDR 	|= 3 << led*2;	// High speed
	port->PUPDR 	|= 1 << led*2;	// Pull-up

	while (true)
    {
		port->BSRR |= 1 << led;			// Output "1"
		softDelay(400000);
		port->BSRR |= 1 << (led + 16);	// Output "0"
		softDelay(400000);
	}
}
