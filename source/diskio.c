/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* Declarations of disk functions */
#include "spiflash_if.h"
#include <stdio.h>
#include <string.h>

#define SECTOR_SIZE  512
#define SECTOR_COUNT 4096 - 1024 // Full disk size - 1024 bytes for raw data
#define BLOCK_SIZE   4096

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    (void)pdrv;
    return !STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    (void)pdrv;
    return !STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv,    /* Physical drive nmuber to identify the drive */
                  BYTE *buff,   /* Data buffer to store read data */
                  LBA_t sector, /* Start sector in LBA */
                  UINT count    /* Number of sectors to read */
) {
    (void)pdrv;
    for (UINT i = 0; i < count; i++) {
        spif_read((uint32_t)sector * SECTOR_SIZE + i * SECTOR_SIZE, SECTOR_SIZE, &buff[i * SECTOR_SIZE]);
    }

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(BYTE pdrv,        /* Physical drive nmuber to identify the drive */
                   const BYTE *buff, /* Data to be written */
                   LBA_t sector,     /* Start sector in LBA */
                   UINT count        /* Number of sectors to write */
) {
    (void)pdrv;
    const uint32_t sector_size = BLOCK_SIZE;
    uint8_t buffer[sector_size];
    for (UINT i = 0; i < count; i++) {
        uint32_t addr = sector * SECTOR_SIZE + i * SECTOR_SIZE;
        uint32_t offset = addr % sector_size;
        uint32_t offset_addr = addr - offset;

        spif_read(offset_addr, sector_size, buffer);

        memcpy(&buffer[offset], &buff[i * SECTOR_SIZE], SECTOR_SIZE);

        spif_erase(offset_addr, sector_size);

        spif_write(offset_addr, sector_size, buffer);
    }
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
                   BYTE cmd,  /* Control code */
                   void *buff /* Buffer to send/receive control data */
) {
    (void)pdrv;
    (void)cmd;
    (void)buff;
    int rc = RES_OK;
    switch (cmd) {
        case CTRL_SYNC:
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SECTOR_COUNT;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *)buff = SECTOR_SIZE;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = BLOCK_SIZE;
            break;
        default:
            printf("unhandled disk_ioctl: %d\r\n", cmd);
            rc = RES_PARERR;
            break;
    }
    return rc;
}
