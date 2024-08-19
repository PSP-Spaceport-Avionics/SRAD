#include "sdCard.h"

FATFS fs; //file system
FIL fil; //file
FRESULT fresult; //store results
char buffer[1024]; //buffer to store data

UINT br, bw;

FATFS *pfs;
DWORD fre_clust;
uint32_t free_space;
uint32_t total;

int buffer_Size (char *buff) {
	int i = 0;
	while (*buff++ != '\0') i++;
	return i;
}
//clear buffer
void buffer_Clear (char *buff) {
	for (int i = 0; i < 1024; i++) {
		buffer[i] = '\0';
	}
}
void cdc_Transmit(char *buff) {
	CDC_Transmit_FS((uint8_t*) buff, strlen(buff));
}

void sd_Start_Up() {
	fresult = f_mount(&fs, "", 1);
	if (fresult == FR_NOT_READY) {
		cdc_Transmit("SD card failed to mount!\r\n");
	} else {
		cdc_Transmit("SD card mounting successful!\r\n");
	}
	//cdc_Transmit("fresult stuck");
	fresult = f_getfree("", &fre_clust, &pfs);
	if (fresult != FR_OK) {
		cdc_Transmit("FR_NOT_READY Error!\r\n");
	} else {
		cdc_Transmit("f_getfree is OK!\r\n");
		total = (uint32_t) ((pfs->n_fatent - 2) * pfs->csize * 0.5);
		sprintf(buffer, "SD CARD Total Size: %lu\r\n", total);
		cdc_Transmit(buffer);
		buffer_Clear(buffer);
		free_space = (uint32_t) (fre_clust * pfs->csize * 0.5);
		sprintf(buffer, "SD CARD Free Space: %lu\r\n", free_space);
		cdc_Transmit(buffer);
		buffer_Clear(buffer);
	}
}

void sd_write(char* input) {
	//temp fix, saving flight temp to different file
	fresult = FR_OK;
	if (input[0] == 'T') {
		fresult = f_open(&fil, "flight_log_time.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND);
	} else {
		fresult = f_open(&fil, "flight_log_gps.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_OPEN_APPEND);

	}
	fresult = f_puts(input, &fil);
	//cdc_Transmit("flight_log1.txt created and data written\r\n");
	fresult = f_close(&fil);
	//sd_read(input);
}

//reads back what's being written onto terminal
void sd_read(char* input) {
	fresult = f_open(&fil, "flight_log1.txt", FA_READ);
	f_gets(input, strlen(input), &fil);
	strcpy(buffer, input);
	strcat(buffer, "\r");
	cdc_Transmit(buffer);
	fresult = f_close(&fil);
	buffer_Clear(buffer);
}