/*
 * bootloader.h
 *
 *  Created on: Sep 11, 2022
 *      Author: suraj
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include"main.h"

typedef enum {
	BL_OK,
	BL_ERROR
}BL_StatusTypedef;


typedef struct bl_signature{
	float bl_version;
	float app_version;
	int update_flag;	//1 if update available : 0 if not
	struct {
		uint8_t day;
		uint8_t month;
		uint16_t year;
	}time_stamp;
}bl_sig_t;


//macros
#define BL_UART huart5
#define BL_INPUT_TIMEOUT 5000U
#define CMD_SIZE 1

//command code
#define BL_JMP_TO_USER_CODE 		'0'
#define BL_WRITE_BIN_TO_MEMORY 		'1'
#define BL_GET_VERSION 				'2'


/*user code section*/
#define FLASH_APPLICATION_AREA 0x08010000U
#define RESET_HANDLER_ADDRESS 0x08010004U

//download area
#define FLASH_DOWNLOAD_AREA 0x08080000U
#define FLASH_SIGNATURE_AREA 0x0800C000U
//function prototypes
void bootloader_mode(void);

//bootloader command functions
BL_StatusTypedef bootloader_jump_to_user_code(UART_HandleTypeDef*);
BL_StatusTypedef bootloader_get_bl_version(UART_HandleTypeDef*);
BL_StatusTypedef bootloader_write_bin_to_memory(uint8_t *,int );
BL_StatusTypedef bootloader_read_memory_sector(UART_HandleTypeDef*);
/*bootloader flash API*/
BL_StatusTypedef bootloader_flash_erase_all(UART_HandleTypeDef*);
BL_StatusTypedef bootloader_flash_erase_download_area();
BL_StatusTypedef bootloader_flash_erase_application_area();
BL_StatusTypedef bootloader_flash_erase_signature_area();
BL_StatusTypedef bootloader_flash_varify(UART_HandleTypeDef*);

BL_StatusTypedef bootloader_app_update(UART_HandleTypeDef*);
/*API used for signature area*/
BL_StatusTypedef bootloader_update_signature_set_flag(UART_HandleTypeDef*);
BL_StatusTypedef bootloader_update_signature_reset_flag(UART_HandleTypeDef*);
BL_StatusTypedef bootloader_update_signature_app_version(UART_HandleTypeDef*);
BL_StatusTypedef bootloader_update_signature_bl_version(UART_HandleTypeDef*);
BL_StatusTypedef bootloader_signature_update(bl_sig_t *sigdata);
int bootloader_signature_get_reset_flag(UART_HandleTypeDef*);
float bootloader_signature_get_app_version(UART_HandleTypeDef*);
float bootloader_signature_get_bl_version(UART_HandleTypeDef*);

BL_StatusTypedef bootloader_unlock_flash();
BL_StatusTypedef bootloader_lock_flash();


#endif /* INC_BOOTLOADER_H_ */
