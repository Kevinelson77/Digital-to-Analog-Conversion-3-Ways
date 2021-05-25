// Kevin Nelson
// Embedded Systems - 10k potentiometer ADC to LTC1661 DAC via SPI
// Spring 2021

#include "stm32f4xx.h"

void Port_init(void);
void Grab_Conversion_Data(void); 
void SPI1_write(unsigned int data);
static float result;
		
int main (void) {
		Port_init();
		
    while (1) {
				Grab_Conversion_Data();
				SPI1_write(result);
		}
}
	
// Port Initializations for LCD. PA5-R/S, PA6-R/W, PA7-EN, PB0-PB7 for D0-D7, respectively.
void Port_init(void) {
		//Enable Clocks
		RCC->AHB1ENR = 1;	            	// enable GPIOA clocks
		RCC->APB2ENR |= 0x00001100;     // enable ADC1/SPI1 clock
		
		// Enable Modes
		GPIOA->MODER = 0;    						// clear pin mode
		GPIOA->MODER = 0x5555995C;    	// set pins output mode, PA1 to analog, PA5 for SCLK/PA7 for MOSI via alt function mode
		GPIOA->AFR[0] &= ~0xF0F00000;   // clear alt mode
    GPIOA->AFR[0] |= 0x50500000;   	// set alt mode SPI1
	
		// Setup ADC1 
    ADC1->CR2 = 0;                  // SW trigger
    ADC1->SQR3 = 1;                 // conversion sequence starts at ch 1
    ADC1->SQR1 = 0;                 // conversion sequence length 1
    ADC1->CR2 |= 1;                 // enable ADC1
	
		// Set SPI Control Register
    SPI1->CR1 = 0x31C;		   				// set the Baud rate, 16-bit data frame
    SPI1->CR2 = 0;
    SPI1->CR1 |= 0x40;              // enable SPI1 module
}

// Enables slave select, writes one byte to SPI1, wait for transmission complete and deassert slave select
void SPI1_write(unsigned int data) {
    while (!(SPI1->SR & 2)) {}      // wait until Transfer buffer Empty
    GPIOA->BSRR = 0x00100000;       // assert slave select
    SPI1->DR = data;  // write command and data
    while (SPI1->SR & 0x80) {}      // wait for transmission done
    GPIOA->BSRR = 0x00000010;       // deassert slave select			
}

// Start & Grab ADC conversion Data
void Grab_Conversion_Data(void){
		ADC1->CR2 |= 0x40000000;        															// start a conversion
		while(!(ADC1->SR & 2)) {}       															// wait for conv complete
		result = ADC1->DR;              															// read conversion result		
}
