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


char TimeString[20] = "20170804 16:00:00";

char File_name[]={"0:/20170314.txt"};

extern unsigned char time_buf[8];



//创建文件路径
void Create_filename()
{
	//u32 Date,Time;
	char string0[21] = {"0:/"};		     //根目录
	char string1[11]  = {"       "};	 //目录
	char string2[5]  = {".txt"};	     //文件类型
	//文件名合成
	string1[0]=((time_buf[0]>>4)&0x0f)+0x30;	    //2
	string1[1]=(time_buf[0]&0x0f)+0x30;   	        //0
	string1[2]=((time_buf[1]>>4)&0x0f)+0x30;	    //1
	string1[3]=(time_buf[1]&0x0f)+0x30;			    //7
	string1[4]='-';
	string1[5]=((time_buf[2]>>4)&0x0f)+0x30;		//0
	string1[6]=(time_buf[2]&0x0f)+0x30; 		    //3
	string1[7]='-';
	string1[8]=((time_buf[3]>>4)&0x0f)+0x30;		//0
	string1[9]=(time_buf[3]&0x0f)+0x30; 		    //6

	strcat(string0,string1);	//合成文件路径
	strcat(string0,string2);
	strcpy(File_name,string0);
}
void Write_dataToTxt(const char *file_path,char *dat)
{
	FRESULT res;
	FIL fsrc;
	FATFS fs;
	uint16_t bw;

#if DEBUG
	OSBsp.Device.Usart2.WriteString("write file ......\n\r");
#endif
	res=f_mount(0, &fs);
	res=f_open(&fsrc,(char *)file_path, FA_OPEN_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		OSBsp.Device.Usart2.WriteString("open file error\n\r");
	}
	else
	{
#if DEBUG
		OSBsp.Device.Usart2.WriteString("open file OK!\n\r");
#endif
		f_lseek(&fsrc,fsrc.fsize);                      //移动指针到末尾
		res = f_write(&fsrc, dat, strlen(dat), &bw);    /* Write it to the dst file */
#if DEBUG
		if(res == FR_OK)
		{
			unsigned int num_w;
			num_w=strlen(dat);
			SendByteToUart2(num_w/100+0x30);
			SendByteToUart2(num_w%100/10+0x30);
			SendByteToUart2(num_w%10+0x30);
			OSBsp.Device.Usart2.WriteString(" bytes has been writted!\r\n");
			OSBsp.Device.Usart2.WriteString("write data ok! \n\r");
		}
		else
		{
			OSBsp.Device.Usart2.WriteString("write data error \n\r");
		}
#endif
		/*close file */
		f_close(&fsrc);
	}
	f_mount(0,NULL);
}









