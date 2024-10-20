/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "bsp/sdcard/sdcard.h"

/* It looks ok to use a global variable here, because this lib allows only
 * one SD card at a time anyway. */
extern mart::Sdcard sdcard;

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE /*pdrv*/	/* Physical drive nmuber to identify the drive */
)
{
  return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE /*pdrv*/	/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE /*pdrv*/,	/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	if(sdcard.read_begin(sector) < 0) {
        return RES_ERROR;
    }

    while(count > 0) {
        if(sdcard.read_data(buff) < 0) {
            return RES_ERROR;
        }
        buff += 512;
        count--;
    }

    if(sdcard.read_end() < 0) {
        return RES_ERROR;
    }

    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE /*pdrv*/,		/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if(sdcard.write_begin(sector) < 0) {
        return RES_ERROR;
    }

    while(count > 0) {
        if(sdcard.write_data(buff) < 0) {
            return RES_ERROR;
        }

        buff += 512;
        count--;
    }

    if(sdcard.write_end() < 0) {
        return RES_ERROR;
    }

    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE /*pdrv*/,	/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void */*buff*/	/* Buffer to send/receive control data */
)
{
	if(cmd == CTRL_SYNC) {
        return RES_OK;
    } else {
        // should never be called
        return RES_ERROR;
    }

	return RES_PARERR;
}
