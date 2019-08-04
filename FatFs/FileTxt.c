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
* Filename      : FileTxt.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include <bsp.h>



//创建文件路径
void g_SD_FileName_Creat(const char *docPath,uint8_t *date,char *filename)
{
	char date_string[7];
	date_string[0]=((date[1]>>4)&0x0f)+0x30;	    //1
	date_string[1]=(date[1]&0x0f)+0x30;			    //7
	date_string[2]=((date[2]>>4)&0x0f)+0x30;		//0
	date_string[3]=(date[2]&0x0f)+0x30; 		    //3
	date_string[4]=((date[3]>>4)&0x0f)+0x30;		//0
	date_string[5]=(date[3]&0x0f)+0x30; 		    //6

	sprintf(filename,"%s%s.txt",docPath,date_string);
	g_Printf_dbg("%s : %s\n\r",__func__,filename);
}

void g_SD_File_Write(const char *file_path,const char *dat)
{
	FRESULT res;
	FIL fsrc;
	FATFS fs;
	uint16_t bw;

	g_Printf_dbg("write file to %s......\n\r",file_path);
	res=f_mount(0, &fs);
	res=f_open(&fsrc,(char *)file_path, FA_OPEN_ALWAYS | FA_WRITE);
	if(res != FR_OK){
		uint32_t res_s = res;
		g_Printf_dbg("open file error = %d\n\r",res_s);
	}else{
		g_Printf_dbg("open file OK!\n\r");
		f_lseek(&fsrc,fsrc.fsize);                      //移动指针到末�?
		res = f_write(&fsrc, dat, strlen(dat), &bw);    /* Write it to the dst file */
		if(res == FR_OK){
			uint32_t num_w;
			num_w=strlen(dat);
			g_Printf_info("%d bytes has been writted!\r\n",num_w);
			g_Printf_info("write data ok! \n\r");
		}else{
			g_Printf_dbg("write data error \n\r");
		}
		/*close file */
		f_close(&fsrc);
	}
	f_mount(0,NULL);
}









