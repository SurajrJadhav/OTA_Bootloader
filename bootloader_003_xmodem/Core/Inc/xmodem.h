/*
 * xmodem.h
 *
 *  Created on: 15-Sep-2022
 *      Author: suraj
 */

#ifndef INC_XMODEM_H_
#define INC_XMODEM_H_

#include"main.h"

typedef enum{
	XMODEM_OK,
	XMODEM_ERROR
}XMODEM_StatusTypedef;


#define SOH		0x01
#define STX		0x02
#define ACK		0x06
#define NAK		0x15
#define EOT		0x04
#define ETB		0x17
#define CANCEL	0x18
#define X_C		0x43

#define PKT_SIZE 128
#define PKT_SIZE_1K 1024

#define XMODEM_RX_WAIT_TIME 3000U


/*API used for xmodem*/
XMODEM_StatusTypedef xmodem_receive(UART_HandleTypeDef *BL_UART);
uint16_t xmodem_calcrc(unsigned char *ptr, int count);
uint8_t xmodem_ready_to_receive(UART_HandleTypeDef *BL_UART);
uint8_t xmodem_ready_to_receive_after_NAK(UART_HandleTypeDef *BL_UART);
#endif /* INC_XMODEM_H_ */
