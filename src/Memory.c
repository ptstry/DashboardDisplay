/*
 * Memory.c
 *
 * Created: 12/06/2018 18:26:26
 *  Author: Piotr
 *  Description: Copy the SD card content to SDRAM
 */ 

#include <asf.h>
#include <string.h>

#include "Memory.h"

#define DATA_SIZE 2048 // not less than 64
#define OBJECT_SIZE 16
static uint8_t buffer[DATA_SIZE];
static uint32_t *ramptr = (uint32_t *)BOARD_SDRAM_ADDR;
static uint32_t *objptr[OBJECT_SIZE];

//volatile uint32_t mask = 0b11011111010110000011111111111111;



void set_GAR() {
    GAR[0] = convert_color(0x238823);
    //GAR[0] = convert_color(0x2300FF);
    GAR[1] = convert_color(0x00BFFF);
    GAR[2] = convert_color(0x2D22D2);
    //GAR[2] = convert_color(0x00FF00);
    B = convert_color(0xFF9A2E);
}

uint32_t convert_color(uint32_t color) {
    uint32_t res=0, tmp=0;
    res = color & 0x3FFF; // take first 14 bits
    //color = color >> 14;
    tmp |= (color >> 22) & 0x3;
    tmp = tmp << 6;
    tmp |= (color >> 17) & 0x1F;
    tmp = tmp << 2;
    tmp |= (color >> 16) & 0x1;
    tmp = tmp << 3;
    tmp |= (color >> 14) & 0x3;
    tmp = tmp << 19;
    res |= tmp;
    //res |= (1<<21); // Ensure that Data Enable bit is high
    return res;
}

void memory_copy_file(char *file_name);

void sdram_test() {
    uint32_t *pul = (uint32_t *)BOARD_SDRAM_ADDR;
    volatile uint32_t val = 0;
    for (uint32_t i=0; i<100; i++) {
        *pul = val;
        pul++;
        val += 2;
    }
    pul = (uint32_t *)BOARD_SDRAM_ADDR;
    for (uint32_t i=0; i<100; i++) {
        val = pul[i];
        printf("%u ", pul[i]);
    }
}

void memory_copy_file(char *file_name) {
    FRESULT res;
    FIL file_object;
    objptr[0] = ramptr;
    res = f_open(&file_object, (char const *)file_name,
        FA_OPEN_EXISTING | FA_READ);
        
    if (res != FR_OK) {
        printf("-E- f_open read pb: 0x%X\n\r", res);
        return;
    }
    memset(buffer, 0, DATA_SIZE);
    uint32_t byte_to_read = file_object.fsize;
    UINT byte_read; // number of bytes read
    
    for (uint32_t i=0; i<byte_to_read; i+=DATA_SIZE) {
        res = f_read(&file_object, buffer, DATA_SIZE, &byte_read);
        if (res != FR_OK) {
            printf("-E- f_read pb: 0x%X\n\r", res);
            break;
        }
        
        /* Write the buffer to SDRAM */        
        int j=0;
        if(i==0)
            j = 0x36; // first byte containing color
        for(j; j<DATA_SIZE; j++) {
            *ramptr = buffer[j];
            ramptr++;
            //printf("%d ", buffer[j]);
        }
        //printf("\n\r");
        
        
    }
    
    return;
}

void memory_copy() {
    // go through all objects stored on SD card
    // naming scheme: 0.bmp, 0.conf, ...
    printf("Memory copy\n\r");
    char file1name[] = "0:0.bmp";
    char file2name[] = "0:0.conf";
    Ctrl_status status;
    volatile FRESULT res = FR_OK;
    FATFS fs;
    FIL file_object;
    FILINFO* fno = NULL;
    
    do {} while (sd_mmc_check(0));
    
    printf("Mount disk (f_mount)...\r\n");
    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
    if (FR_INVALID_DRIVE == res) {
        printf("[FAIL] res %d\r\n", res);
        return; // return with error
    }
    printf("[OK]\r\n");
    
    for (int i=0; 1; i++) {
        //res = f_stat(file1name, fno);
        res = f_open(&file_object, (char const *)file1name,
            FA_OPEN_EXISTING | FA_READ);
        if(res!=FR_OK)
            break; // break with error
        
        memory_copy_file(file1name); // copy pure dataFRESULT res;
        
        objptr[i+1] = ramptr;
        /*
        for(uint32_t *i=objptr[0]; i!=ramptr; i++) {
            nop();
            printf("%d ", *i);
            nop();
        }
        */
        sprintf(file1name, "%d.bmp", i+1);
        sprintf(file2name, "%d.conf", i+1);
    }
}