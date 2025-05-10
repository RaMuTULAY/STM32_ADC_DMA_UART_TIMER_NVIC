
#include "main.h"
#include "USER_RS485_20221019_1900.h"

#include "USER_ADC_20221019_1900.h"
#include "USER_Buttons_20221019_1900.h"

extern UART_HandleTypeDef huart1;
//----------------------------UART------------------------------------------

#define UART1RXBUFSIZE	10		//needs to be changed according to maximum allowed message lenght of USER sended data
#define	UART1TXBUFSIZE	10
__attribute__((aligned(32))) volatile  char Uart1Rx_buf[UART1RXBUFSIZE] = {'0','1','2','3','4','5','6','7','\r','\n'};  //  creating a buffer of MAX RX Message lenght bytes
__attribute__((aligned(32))) volatile  char Uart1Tx_buf[UART1TXBUFSIZE] = {'R','x','2','3','4','5','6','7','\r','\n'};  //  creating a buffer of MAX TX Message lenght bytes

char	UartRxTemp[UART1RXBUFSIZE];


uint8_t	Uart1RxWDog = 0;
uint8_t	Uart1TxWDog = 0;
bool u3rxcompleted = 0;
//----------------------------UART------------------------------------------


//----------------------------UART------------------------------------------
/**
  * @brief  UART error callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
	if(huart == &huart1){

		//	uint16_t 	temp1 = huart->Instance->DR;
		//	uint16_t 	temp2 = huart->Instance->SR;

		UserUartInit(&huart1);
	}
}

//-------------------------------START UART RECEIVE-Transmit DMA	-------------------------------------------
void UserUartInit(UART_HandleTypeDef *huart){
		  HAL_UART_Receive_DMA (huart, Uart1Rx_buf, 1);  // Receive 1 Byte of data

}
//-------------------------------START UART RECEIVE-Transmit  DMA	-------------------------------------------

//----------------------------UART RxHalf------------------------------------------
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1){

	}
}
//----------------------------UART RxHalf------------------------------------------

bool WaitingForHeader = 1;
bool	DataValid = 0;
uint8_t	UartRxNext=0;
uint8_t	UartRxInstanceEnd=2;
uint8_t	UartRxInstanceCRC=0;
uint8_t	UartRxInstanceKomut=0;

//----------------------------UART RxCplt------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if(huart == &huart1){

		if (WaitingForHeader){
			if (Uart1Rx_buf[0] == HEADERBYTE){
				WaitingForHeader = 0;
				UartRxTemp[0] = Uart1Rx_buf[0];
				UartRxNext = 0;
			}
		}
		else{

			UartRxNext++;

			if (UartRxNext == 1){
				UartRxInstanceEnd = Uart1Rx_buf[0];		//Data Uzunluk (Header Hariç, Data Uzunluk Byte + CRC uzunluk Byte (1) olarak alındı, toplam güncel paket uzunluğu)
				if (UartRxInstanceEnd > UART1RXBUFSIZE) {UartRxInstanceEnd = UART1RXBUFSIZE;}
				UartRxTemp[UartRxNext] = Uart1Rx_buf[0];
			}

			if( (UartRxNext > 1) & (UartRxNext < UartRxInstanceEnd) ){
				UartRxTemp[UartRxNext] = Uart1Rx_buf[0];

			}


			if (UartRxNext == UartRxInstanceEnd){
				UartRxInstanceCRC = Uart1Rx_buf[0];
				UartRxTemp[UartRxNext] = Uart1Rx_buf[0];

				uint8_t	CalcRxCrc = 0;
				for(uint8_t i=0; i<UartRxInstanceEnd; i++){
				CalcRxCrc += UartRxTemp[i];
				}

				if(CalcRxCrc == UartRxInstanceCRC){
					DataValid = 1;
					UartRxInstanceKomut = UartRxTemp[2];
				}

				WaitingForHeader = 1;		//this instance is ended we need to wait for the next header and package
			}

		}

		if(DataValid & (UartRxInstanceKomut == 0x20)){
			Uart1Tx_buf[0] = HEADERBYTE;								//Header
			Uart1Tx_buf[1] = UART1TXBUFSIZE;							//Data Uzunluk
			Uart1Tx_buf[2] = (uint8_t)( Sensor1_ADC & 0x00FF);			//Sensor1 Gerilim LSB
			Uart1Tx_buf[3] = (uint8_t)((Sensor1_ADC & 0xFF00) >> 8 );	//Sensor1 Gerilim MSB
			Uart1Tx_buf[4] = (uint8_t)( Sensor2_ADC & 0x00FF);			//Sensor2 Gerilim LSB
			Uart1Tx_buf[5] = (uint8_t)((Sensor2_ADC & 0xFF00) >> 8 );	//Sensor2 Gerilim MSB
			Uart1Tx_buf[6] = LED_1_State;								//Led1 Durum
			Uart1Tx_buf[7] = LED_2_State;								//Led2 Durum
			Uart1Tx_buf[8] = 0x21;										//Komut tipi 0x21
			uint8_t	CalcTxCrc = 0;
			for(uint8_t i=0; i<UART1TXBUFSIZE-1; i++){					// if Tx Message lenght is not Constant this function needs to be MODIFIED
			CalcTxCrc += Uart1Tx_buf[i];
			}
			Uart1Tx_buf[9] = CalcTxCrc;		//CRC

			HAL_UART_Transmit_DMA(&huart1, (uint8_t*) &Uart1Tx_buf, UART1TXBUFSIZE);  // Transmit UARTTXBUFSIZE Bytes of data
			DataValid = 0;
		}
	}

//----------------------------if you want to use HAL_UART_Transmit_DMA in main Start------------------------
////at UART1 callback interrupt
//	if(huart == &huart3){
//		u1rxcompleted = 1;
//	}
////at uart1 callback interrupt
	//at main
	//	  if(u1rxcompleted){
	//			u1rxcompleted = 0;
	//			for(uint8_t tmp=0;tmp<=UART1TXBUFSIZE;tmp++)	{Uart1Tx_buf[tmp] = Uart1Rx_buf[tmp];}		//place some data to tx buf / echo rxbuf
	//			HAL_UART_Transmit_DMA(&huart1, (uint8_t*) &Uart1Tx_buf, UART1TXBUFSIZE);  // Transmit UART1TXBUFSIZE Bytes of data
	//
	//	  }
	//at main
//----------------------------if you want to use HAL_UART_Transmit_DMA in main end--------------------------


}
//----------------------------UART RxCplt------------------------------------------

//----------------------------UART TxHalf------------------------------------------
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1){

	}
}
//----------------------------UART TxHalf------------------------------------------

//----------------------------UART TxCplt------------------------------------------
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1){

	}
}
//----------------------------UART TxCplt------------------------------------------


/*
Valid Message Examples (User can send this messages to get Button, led, sensor values)

0x55 0x03 0x20 0x78
0x55 0x04 0x20 0x00 0x79
0x55 0x05 0x20 0x00 0x00 0x7A
0x55 0x06 0x20 0x00 0x00 0x00 0x7B
0x55 0x07 0x20 0x00 0x00 0x00 0x00 0x7C
0x55 0x08 0x20 0x00 0x00 0x00 0x00 0x00 0x7D
0x55 0x09 0x20 0x00 0x00 0x00 0x00 0x00 0x00 0x7E
*/

