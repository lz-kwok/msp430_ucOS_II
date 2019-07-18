/*
*********************************************************************************************************
*
*                          (c) Copyright 2003-2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*
*                                      Texas Instruments MSP430
*                                               on the
*                                          MSP-EXP430F5259LP
*                                          Evaluation Board
*
* Filename      : Diskio.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include <bsp.h>



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
		uint8_t drv				/* Physical drive nmuber (0..) */
)
{
	uint8_t state;
	state=SD_Init();
	if(!state){
	 return STA_NODISK;
	}   

	return 0;									  
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
		uint8_t drv		/* Physical drive nmuber (0..) */
)
{ /**
    if(drv)
    {
        return STA_NOINIT;  //仅支持磁盘0操作
    }

    //检查SD卡是否插入
    if(!SD_DET())
    {
        return STA_NODISK;
    }
    return 0;
	**/
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
		uint8_t drv,		/* Physical drive nmuber (0..) */
		uint8_t *buff,		/* Data buffer to store read data */
		uint32_t sector,	       /* Sector address (LBA) */
		uint8_t count		/* Number of sectors to read (1..255) */
)
{
	uint8_t res=0;
	/*
    if (drv || !count)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
    }
    if(!SD_DET())
    {
        return RES_NOTRDY;  //没有检测到SD卡，报NOT READY错误
    }

    */
	
    if(count==1)            //1个sector的读操作
    {
        res = SD_ReadSingleBlock(sector, buff);
    }
    else                    //多个sector的读操作
    {                                                
        res = SD_ReadMultiBlock(sector, buff, count);
    }                                                
	/*
    do                           
    {                                          
        if(SD_ReadSingleBlock(sector, buff)!=0)
        {                                      
            res = 1;                           
            break;                             
        }                                      
        buff+=512;                             
    }while(--count);                                         
    */
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
		uint8_t drv,			/* Physical drive nmuber (0..) */
		const uint8_t *buff,	/* Data to be written */
		uint32_t sector,		/* Sector address (LBA) */
		uint8_t count			/* Number of sectors to write (1..255) */
)
{
	uint8_t res;
/*
    if (drv || !count)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
    }
    if(!SD_DET())
    {
        return RES_NOTRDY;  //没有检测到SD卡，报NOT READY错误
    }
*/
    // 读写操作
    if(count == 1)
    {
        res = SD_WriteSingleBlock(sector, buff);
    }
    else
    {
        res = SD_WriteMultiBlock(sector, buff, count);
    }
    // 返回值转换
    if(res == 0)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
		uint8_t drv,		/* Physical drive nmuber (0..) */
		uint8_t ctrl,		/* Control code */
		void *buff			/* Buffer to send/receive control data */
)
{
    DRESULT res;


    if (drv)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，否则返回参数错误
    }
    
    //FATFS目前版本仅需处理CTRL_SYNC，GET_SECTOR_COUNT，GET_BLOCK_SIZ三个命令
    switch(ctrl)
    {
    case CTRL_SYNC:
	/*
         SD_CS_ENABLE();
	 
        if(SD_WaitReady()==0)
        {
            res = RES_OK;
        }
        else
        {
            res = RES_ERROR;
        }
        SD_CS_DISABLE();
		*/
		res=RES_OK;
        break;
        
    case GET_BLOCK_SIZE:
        *(int16_t*)buff = 512;
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:
        *(uint32_t*)buff = SD_GetCapacity();
        res = RES_OK;
        break;
    default:
        res = RES_PARERR;
        break;
    }

    return res;
}


/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/* 31-26: Year(0-127 org.1970), 25-22: Month(1-12), 21-17: Day(1-31) */                                                                                                                                                                                                                                          
/* 16-12: Hour(0-23), 11-6: Minute(0-59), 5-0: Second(0-60) */                                                                                                                                                                                                                                                
uint32_t get_fattime (void)
{

	uint32_t date=0;
//    timer.w_year -= 1970;	//年份为1970年起       
//  
//    date = (timer.w_year << 26)|(timer.w_month<<22)|(timer.w_date<<17)|\
//            (timer.hour<<12)|(timer.min<<6)|(timer.sec);

    return date;
	
}


