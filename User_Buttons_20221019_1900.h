
#include "main.h"
#include "stdbool.h"

extern	  bool	LED_1_State;
#define	LEDON	1

void UserButtonInit();


#define USER_BUTTON_INPUT_PRESSED_STA 0	//what is the input pin state when the button is pressed
#define	USER_BUTTON_DEBOUNCE_TIME 50	//as milliseconds
#define	BUTTON_PRESSED 1				//use this in the main program flow, 1 means button press is detected

typedef struct{
	GPIO_TypeDef *ButtonGPIO_Port;
	uint16_t	ButtonPin;
	uint32_t	SBxPressedCnt;
	uint32_t	SBxReleasedCnt;
	bool		SBx_State;			//if this is equal to "BUTTON_PRESSED" it means button press is detected, needs to be cleared by user after read

}User_Button;


extern	User_Button	SensorButton_1;
extern	User_Button	SensorButton_2;

