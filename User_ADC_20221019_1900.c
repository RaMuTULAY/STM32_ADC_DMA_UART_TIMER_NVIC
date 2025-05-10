

#include "USER_ADC_20221019_1900.h"

#include "USER_Buttons_20221019_1900.h"
#include "USER_RS485_20221019_1900.h"

uint32_t Sensor1_ADC,Sensor2_ADC;

	bool	adc_stopped = !ADC_STOP;

	bool	Sensor1ADCINState = SENSORADC_ACTIVE;
	bool	Sensor2ADCINState = SENSORADC_ACTIVE;

	bool	LED_2_State = LEDON;


#warning ADC must be configured -ADC_DATAALIGN_RIGHT- for not to overflow total_x register.

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	uint32_t total_1,total_2;
	uint32_t i= 0;

	total_1 = 0;
	total_2 = 0;
	for(i=0;i<ADCRESSIZE;i++){
	total_1 += ADC_RES[i++] ;		//needs to be rearrenged acc to ADCNUMBEROFCHANNELS
	total_2 += ADC_RES[i] ;
	}

	Sensor1_ADC = (total_1 / (ADCRESSIZE/2) );
	Sensor2_ADC	= (total_2 / (ADCRESSIZE/2) );

	  if ( (Sensor1ADCINState == !SENSORADC_ACTIVE) ){
		  Sensor1_ADC = 0;
	  }
	  if ( (Sensor2ADCINState == !SENSORADC_ACTIVE) ){
		  Sensor2_ADC = 0;
	  }

	  if ( (Sensor1ADCINState == !SENSORADC_ACTIVE) & (Sensor2ADCINState == !SENSORADC_ACTIVE) ){
		  HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);  //PWM(No Output) ADC Trigger Timer //stop ADC Trigger Timer so ADC Read will be stopped
		  HAL_ADC_Stop_DMA(&hadc1);
		  adc_stopped = ADC_STOP;
	  }


	if( (Sensor1_ADC > Sensor1_ADCThreshold) | (Sensor2_ADC > Sensor2_ADCThreshold) ){
		HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
		LED_2_State = LEDON;
	}
	else{
		HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
		LED_2_State = !LEDON;
	}

}








//----------------------------------------
void UserADCInit(){                   //ADC & ADC Trigger Timer , ADC DMA, INIT



  HAL_ADCEx_Calibration_Start(&hadc1);	// Calibrate The ADC On Power-Up For Better Accuracy

  HAL_ADC_Start_DMA(&hadc1, &ADC_RES[0], ADCRESSIZE);
  HAL_ADC_Start_IT(&hadc1);
  //------------------------------------INIT ADC Trigger Timer--------------------------------------------------------------
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);  //PWM(No Output) ADC Trigger Timer ON
  //------------------------------------ ADC Trigger Timer------------------------------------------


}
