/*
 * xmodem.c
 *
 *  Created on: 15-Sep-2022
 *      Author: suraj
 */

#include "xmodem.h"
#include "bootloader.h"

UART_HandleTypeDef huart4;

#define SWAP_BYTE(x) (x=((x&0x00ff)<<8) | ((x&0xff00)>>8))

#define DEBUG_XMODEM
/*
 * 		Packet Info
*      +-----+-------+-------+------+-----+------+
*      | SOH | PKT_num | !pkt_num | data |  CRC  |
*      +-----+-------+-------+------+-----+------+
*/

uint16_t xmodem_calcrc(unsigned char *ptr, int count)
{
	uint16_t  crc;
    char i;

    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    SWAP_BYTE(crc);
    return (crc);
}

uint8_t xmodem_ready_to_receive(UART_HandleTypeDef *BL_UART){
	uint8_t header=0,response=X_C;

	while(!(header == STX || header == SOH)){
		/*send 'C' to receive as CRC*/
		HAL_UART_Transmit(BL_UART,&response, 1, HAL_MAX_DELAY);
		/*Receive & Check Header**/
		HAL_UART_Receive(BL_UART, &header, 1, XMODEM_RX_WAIT_TIME);
	}
	return header;
}

/*if NAK received call this at the time of receiving header*/
uint8_t xmodem_ready_to_receive_after_NAK(UART_HandleTypeDef *BL_UART){
	uint8_t header=0,response=NAK;
	while(!(header == STX || header == SOH)){
		HAL_UART_Transmit(BL_UART,&response, 1, HAL_MAX_DELAY);
		/*Receive & Check Header**/
		HAL_UART_Receive(BL_UART, &header, 1, XMODEM_RX_WAIT_TIME);
	}
	return header;
}

XMODEM_StatusTypedef xmodem_receive(UART_HandleTypeDef *BL_UART){
#ifdef DEBUG_XMODEM
	HAL_UART_Transmit(&huart4, "receiving packet...\n\r", strlen("receiving packet...\n\r"), HAL_MAX_DELAY);
#endif

	uint8_t rxbuf[1050]={0};
	uint8_t header,response;

	static uint8_t received_packet_number=0,comp_packet_number,packet_number=0;
	int size,NAK_Flag=0;
	uint16_t received_crc=0;

	packet_number++;
	/*try to receive header in differenr context*/
retry:
	if((packet_number)==1){
		/*reception of first packet*/
		header=xmodem_ready_to_receive(BL_UART);
	}
	else if(NAK_Flag){
		/*reception of header if NAK is sent*/
		header=xmodem_ready_to_receive_after_NAK(huart5);
		NAK_Flag=0;
	}
	else{
		/*reception of header if ACK is sent*/
		HAL_UART_Receive(huart5, &header, 1, XMODEM_RX_WAIT_TIME);
	}


	switch(header)
	 {
	 case SOH:
	 case STX:
		 	 if(header==SOH)
		 	 {
		 		 size=PKT_SIZE;
		 	 }
		 	 else if(header==STX)
		 	 {
		 		 size=PKT_SIZE_1K;
		 	 }
		 	 /*receive PKT_num & size*/
		 	 HAL_UART_Receive(BL_UART, &received_packet_number, 1, XMODEM_RX_WAIT_TIME);
		 	 /*size is of 2 byte*/
		 	 HAL_UART_Receive(BL_UART, &comp_packet_number, 1, XMODEM_RX_WAIT_TIME);
		 	 /*receive DATA*/
		 	 HAL_UART_Receive(BL_UART, rxbuf, size, XMODEM_RX_WAIT_TIME);
		 	 /*receive CRC*/
		 	 HAL_UART_Receive(BL_UART, &received_crc, 2, XMODEM_RX_WAIT_TIME);

		 	/*check CRC*/
		 	uint16_t calculated_crc = xmodem_calcrc(rxbuf, size);
		 	 if(calculated_crc != received_crc  && received_packet_number != packet_number){
		 		 /*set NAK Flag and retry packet reception*/
		 		NAK_Flag=1;
		 		goto retry;
		 	 }
		 	 else
		 	 {
		 		//send NAK
		 		response=ACK;
		 		HAL_UART_Transmit(BL_UART, &response, 1, HAL_MAX_DELAY);
		 	 }
		 	 /*write to flash memory*/
		 	 bootloader_write_bin_to_memory(rxbuf,size);

#ifdef DEBUG_XMODEM
		 	/*debug*/
		 	char temp[12];
		 	sprintf(temp,"received packet=%d\n\r",packet_number);
		 	HAL_UART_Transmit(&huart4, temp, strlen(temp), HAL_MAX_DELAY);
	 		/*debug end*/
#endif
		 	break;

	 case EOT:
	 case ETB:
		 /*respond ACK*/
#ifdef DEBUG_XMODEM
		 sprintf(temp,"received EOT\n\r");
		 HAL_UART_Transmit(&huart4, temp, strlen(temp), HAL_MAX_DELAY);
#endif
		 response=ACK;
		 HAL_UART_Transmit(BL_UART,&response, 1, HAL_MAX_DELAY);

		 /*update APP Version*/
		 bootloader_update_signature_app_version(huart5);
		 /*update signature flag*/
		 bootloader_update_signature_set_flag(huart5);

		 /*Receive of EOT will jump to user app*/
#ifdef DEBUG_XMODEM
		 HAL_UART_Transmit(&huart4, "Jump to user", 15, HAL_MAX_DELAY);
#endif
		 /*jump to APP */
		 if(bootloader_jump_to_user_code(&huart4) == BL_ERROR){
			 HAL_UART_Transmit(&huart4, "ERROR", strlen("ERROR"), HAL_MAX_DELAY);
			 while(1);
		 }

	 }
}

