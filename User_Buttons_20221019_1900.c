
#include "main.h"
#include "USER_Buttons_20221019_1900.h"
#include "USER_ADC_20221019_1900.h"


extern TIM_HandleTypeDef htim2;


User_Button	SensorButton_1;
User_Button	SensorButton_2;

void UserButtonInit(){

	SensorButton_1.SBxPressedCnt = 0;
	SensorButton_1.SBxReleasedCnt = 0;
	SensorButton_1.SBx_State = !BUTTON_PRESSED;
	SensorButton_1.ButtonGPIO_Port = SENSOR_BUTTON_1_GPIO_Port;
	SensorButton_1.ButtonPin = SENSOR_BUTTON_1_Pin;

	SensorButton_2.SBxPressedCnt = 0;
	SensorButton_2.SBxReleasedCnt = 0;
	SensorButton_2.SBx_State = !BUTTON_PRESSED;
	SensorButton_2.ButtonGPIO_Port = SENSOR_BUTTON_2_GPIO_Port;
	SensorButton_2.ButtonPin = SENSOR_BUTTON_2_Pin;



	HAL_TIM_Base_Start_IT(&htim2);
}

void button_check(User_Button*);
void Set_ADC_LED_States();

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)		//for low power operation button read may implemented by GPIO_EXTI and timer one pulse mode
{
	if(htim == &htim2) {				//milliseconds interrupt timer for button debounce calculation

		button_check(&SensorButton_1);
		button_check(&SensorButton_2);
		Set_ADC_LED_States();
	}
}




void button_check(User_Button* Btn){

	if(HAL_GPIO_ReadPin(Btn->ButtonGPIO_Port, Btn->ButtonPin) == USER_BUTTON_INPUT_PRESSED_STA ){
		Btn->SBxPressedCnt++;
		Btn->SBxReleasedCnt = 0;
	}
	else{

		Btn->SBxReleasedCnt++;
		if (Btn->SBxReleasedCnt > (USER_BUTTON_DEBOUNCE_TIME * 2) ){
			Btn->SBxPressedCnt = 0;								//discard intermittent cumulative pressed time while button is in released state
			Btn->SBxReleasedCnt = USER_BUTTON_DEBOUNCE_TIME;	//make sure button release time register to not overflow while button is not presed for a long time
		}

	}

	if( (Btn->SBxPressedCnt > USER_BUTTON_DEBOUNCE_TIME) & (Btn->SBxReleasedCnt > USER_BUTTON_DEBOUNCE_TIME) ){
		Btn->SBx_State = BUTTON_PRESSED;			//BUTTON PRESS DETECTED (needs to be cleared by where button pressed information used)
		Btn->SBxPressedCnt = 0;

	}
}




  extern	bool	Sensor1ADCINState;
  extern	bool	Sensor2ADCINState;

  bool	LED_1_State = LEDON;

void Set_ADC_LED_States(){
	//-------------------------------------------------------------
		  if (SensorButton_1.SBx_State == 1){
			  SensorButton_1.SBx_State = !BUTTON_PRESSED;
			  if (Sensor1ADCINState == SENSORADC_ACTIVE){
				  Sensor1ADCINState = !SENSORADC_ACTIVE;
			  }
			  else{
				  Sensor1ADCINState = SENSORADC_ACTIVE;
				  if(adc_stopped == ADC_STOP){
					  adc_stopped = !ADC_STOP;
					  UserADCInit();						//restart ADC and ADC Trigger timer
				  }
			  }

		  }


		  if (SensorButton_2.SBx_State == 1){
			  SensorButton_2.SBx_State = !BUTTON_PRESSED;
			  if (Sensor2ADCINState == SENSORADC_ACTIVE){
				  Sensor2ADCINState = !SENSORADC_ACTIVE;
			  }
			  else{
				  Sensor2ADCINState = SENSORADC_ACTIVE;
				  if(adc_stopped == ADC_STOP){
					  adc_stopped = !ADC_STOP;
					  UserADCInit();						//restart ADC and ADC Trigger timer
				  }
			  }
		  }
	//-------------------------------------------------------------
	//-------------------------------------------------------------

		  if ( (Sensor1ADCINState == SENSORADC_ACTIVE) & (Sensor2ADCINState == SENSORADC_ACTIVE) ){
			  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
			  LED_1_State = LEDON;
		  }
		  else{
			  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
			  LED_1_State = !LEDON;
		  }
	//-------------------------------------------------------------


}

