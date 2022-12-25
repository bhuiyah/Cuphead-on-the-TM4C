// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 11/15/2021 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
	SYSCTL_RCGCADC_R |= 0x0001;   // 1) activate ADC0
	SYSCTL_RCGCGPIO_R |= 0x38;    // 2) activate clock for Port E
	while((SYSCTL_PRGPIO_R&0x08) == 0 ){};  // 3 for stabilization
  GPIO_PORTD_DIR_R &= ~0x04;    // 4) make PD2 input
  GPIO_PORTD_AFSEL_R |= 0x04;   // 5) enable alternate function on PD4
  GPIO_PORTD_DEN_R &= ~0x04;    // 6) disable digital I/O on PD4
  GPIO_PORTD_AMSEL_R |= 0x04;   // 7) enable analog functionality on PD4
// while((SYSCTL_PRADC_R&0x0001) != 0x0001){}; // good code, but not implemented in simulator
	SYSCTL_RCGCADC_R |= 0x01;     //Activate ADC0
  ADC0_PC_R &= ~0xF;
  ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;        // 9) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;      // 10) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;       // 11) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 5;           // 12) set channel
  ADC0_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;         // 14) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
  uint32_t result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
  return result;
}


void DelayMs(uint32_t n){
volatile uint32_t time;
	while(n){
		time = 6665; // 1msec, tuned at 80 MHz
		while(time){
			time--;
		}
		n--;
	}
}

void IO_Init(void) {
 // --UUU-- Code to initialize PF4 and PF2
	SYSCTL_RCGCGPIO_R |= 0x10; // 1) activate clock for Port E
	__nop(); __nop(); __nop(); // allow time for clock to start
// only PF0 needs to be unlocked, other bits can't be locked
	GPIO_PORTE_DIR_R &= ~0x07; // 5) PE1 & PE2 in 
	GPIO_PORTE_DEN_R |= 0x07; // 7) enable digital I/O on PE1 and PE2
}

//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
 // --UUU-- wait for release; delay for 20ms; and then wait for press
	int32_t IO_Touch (void) {
 // --UUU-- wait for release; delay for 20ms; and then wait for press
		uint32_t var;
		volatile uint32_t input = GPIO_PORTE_DATA_R;
		while((input != 0x04) && (input != 0x02) && (input != 0x01)){
    input = GPIO_PORTE_DATA_R;
			
		if(input == 0x04){
			var = 1;
		}
		if(input == 0x02){
			var = 0;
		}
	}
    DelayMs(25);
    while(input != 0){
    input = GPIO_PORTE_DATA_R;
    }
		return var;
	}



