
#include "main.h"
#include "stdbool.h"

#define	ADC_STOP	1
extern	bool	adc_stopped;

#define	SENSORADC_ACTIVE	1

extern uint32_t Sensor1_ADC,Sensor2_ADC;
extern bool	LED_2_State;

void UserADCInit();                   //ADC & ADC Timer , ADC DMA, INIT


extern ADC_HandleTypeDef hadc1;
//extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim3;			//ADC trigger timer

#define ADCSAMPLESIZEPERCHANNEL	10
#define ADCNUMBEROFCHANNELS	2


#define ADCRESSIZE	(ADCSAMPLESIZEPERCHANNEL * ADCNUMBEROFCHANNELS)
//__attribute__((aligned(4)))  volatile uint32_t	ADC_RES[ADCRESSIZE];
__attribute__((aligned(4))) uint32_t	ADC_RES[ADCRESSIZE];



#if( ADCSAMPLESIZEPERCHANNEL > (0xFFFFFFFFUL / 0x1000UL) )
error addition result may exceed total_x register
#endif

