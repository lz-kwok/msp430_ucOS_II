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
* Filename      : g_DeviceSD.c
* Version       : V1.00
* Programmer(s) : GLZ
*********************************************************************************************************
*/
#include <bsp.h>


uint8_t SD_Type=0;              //SD��������

unsigned long cardSize = 0;     //SD������
uint8_t status = 1;             //״̬
uint8_t timeout = 0;            //����

//*************************************************//
//      g_Device_SDCard_Check
//*************************************************//
void g_Device_SDCard_Check(void)
{
	/***********Test SD Card*******************/
	while((status != 0)&(timeout < 100))          //��ʼ��50��MMC/SD���������Ȼ���ش�����룬���ʼ��ʧ�ܣ�������û��SD������
	{
		status = g_MMC_init();
		timeout++;
	}
	if(status != 0)
	{
#if DEBUG
		OSBsp.Device.Usart2.WriteString("Init Failure,Please Check SD!\r\n");
#endif
	}
	else
	{
#if DEBUG
		OSBsp.Device.Usart2.WriteString("Init SD Success!\r\n");
		cardSize =  MMC_ReadCardSize();                 //��SD���Ĵ�����ȡMMC/SD��������С�����س�����
		SDsizeDisplay(cardSize);
#endif
	}
	/***********End*******************/
}

//***********************************************************************
//            SD��������������ʾ����
//***********************************************************************
void SDsizeDisplay(unsigned long Capacity)
{
	static unsigned int SizeQ,SizeB,SizeS,SizeG;
	unsigned long Size;
	OSBsp.Device.Usart2.WriteString("SD Size:");           //��ʾSD������
	Size=Capacity/1024/1024;
	SizeQ=Size/1000;
	SizeB=(Size%1000)/100;
	SizeS=(Size%1000)%100/10;
	SizeG=(Size%1000)%100%10;
	OSBsp.Device.Usart2.WriteData(SizeQ+'0');
	OSBsp.Device.Usart2.WriteData(SizeB+'0');
	OSBsp.Device.Usart2.WriteData(SizeS+'0');
	OSBsp.Device.Usart2.WriteData(SizeG+'0');
	OSBsp.Device.Usart2.WriteString("MB\r\n");  		    //��ʾSD��������λ
}

/*******************************************************************************
* ������	: SD_GetResponse
* ����	: SD����ȡ�豸��Ӧ
* �������  : Response
* ���ز���  : MSD_RESPONSE_FAILURE �� MSD_RESPONSE_NO_ERROR
*******************************************************************************/
uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t ResponseTick = 0xFFF;		//�ȴ�����

	while ((OSBsp.Device.Spi2.WriteReadData(0XFF) != Response) && ResponseTick)
		ResponseTick--;					//�ȴ��õ�׼ȷ�Ļ�Ӧ

	if (ResponseTick==0)
		return MSD_RESPONSE_FAILURE;    //�õ���Ӧʧ��
	else
		return MSD_RESPONSE_NO_ERROR;   //��ȷ��Ӧ
}

/*******************************************************************************
* ������	: SD_GetResponse
* ����	: �ȴ�SD��д�����
* �������  : ��
* ���ز���  : 0���ɹ�
*******************************************************************************/
uint8_t SD_WaitDataReady(void)
{
	uint8_t r1 = MSD_DATA_OTHER_ERROR;
	uint32_t retry = 0;

    do
    {
        r1 = OSBsp.Device.Spi2.WriteReadData(0xFF) & 0X1F;             //������Ӧ
        if(retry==0xfffe)
        	return 1;

		retry++;

		switch (r1)
		{					   
			case MSD_DATA_OK:               //���ݽ�����ȷ��
				r1=MSD_DATA_OK;
				break;  
			case MSD_DATA_CRC_ERROR:        //CRCУ�����
				return MSD_DATA_CRC_ERROR;  
			case MSD_DATA_WRITE_ERROR:      //����д�����
				return MSD_DATA_WRITE_ERROR;  
			default:                        //δ֪����
				r1=MSD_DATA_OTHER_ERROR;
				break;	 
		}   
    }while(r1==MSD_DATA_OTHER_ERROR);       //���ݴ���ʱһֱ�ȴ�

	retry=0;
	while(OSBsp.Device.Spi2.WriteReadData(0XFF)==0)      //��������Ϊ0,�����ݻ�δд���
	{
		retry++;
		//delay_us(10);                     //SD��д�ȴ���Ҫ�ϳ���ʱ��
		if(retry >= 0XFFFFFFFE)
			return 0XFF;                    //�ȴ�ʧ��
	};	    

    return 0;                               //�ɹ�
}	 

/*******************************************************************************
* ������	: SD_SendCommand
* ����	: ��SD������һ������
* �������  :  cmd  ����
			 arg  �������
			 crc  crcУ��ֵ
* ���ز���  : SD�����ص���Ӧ
*******************************************************************************/
uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t r1;
	uint8_t Retry=0;

	P5OUT |= BIT5;

    OSBsp.Device.Spi2.WriteReadData(0xff);			//����д������ʱ
	OSBsp.Device.Spi2.WriteReadData(0xff);     
 	OSBsp.Device.Spi2.WriteReadData(0xff);

    //Ƭѡ���õͣ�ѡ��SD��
    SD_CS_Low();
    //����

    OSBsp.Device.Spi2.WriteReadData(cmd | 0x40);		//�ֱ�д������
    OSBsp.Device.Spi2.WriteReadData(arg >> 24);
    OSBsp.Device.Spi2.WriteReadData(arg >> 16);
    OSBsp.Device.Spi2.WriteReadData(arg >> 8);
    OSBsp.Device.Spi2.WriteReadData(arg);
    OSBsp.Device.Spi2.WriteReadData(crc); 

    //�ȴ���Ӧ����ʱ�˳�
    while((r1=OSBsp.Device.Spi2.WriteReadData(0xFF))==0xFF)
    {
        Retry++;	    
        if(Retry>200)break; 
    }   
    //�ر�Ƭѡ
    SD_CS_High();
    //�������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ���
    OSBsp.Device.Spi2.WriteReadData(0xFF);
    //����״ֵ̬
    return r1;
}

/*******************************************************************************
* ������	: SD_SendCommand_NoDeassert
* ����	: ��SD������һ������(�����ǲ�ʧ��Ƭѡ�����к������ݴ�����
* �������  :  cmd  ����
			 arg  �������
			 crc  crcУ��ֵ
* ���ز���  : SD�����ص���Ӧ
*******************************************************************************/
uint8_t SD_SendCommand_NoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t Retry=0;
	uint8_t r1;

    OSBsp.Device.Spi2.WriteReadData(0xff);//����д������ʱ
	OSBsp.Device.Spi2.WriteReadData(0xff);  	 	 
    SD_CS_Low();//Ƭѡ���õͣ�ѡ��SD��
    //����
    OSBsp.Device.Spi2.WriteReadData(cmd | 0x40); //�ֱ�д������
    OSBsp.Device.Spi2.WriteReadData(arg >> 24);
    OSBsp.Device.Spi2.WriteReadData(arg >> 16);
    OSBsp.Device.Spi2.WriteReadData(arg >> 8);
    OSBsp.Device.Spi2.WriteReadData(arg);
    OSBsp.Device.Spi2.WriteReadData(crc);   

    //�ȴ���Ӧ����ʱ�˳�
    while((r1=OSBsp.Device.Spi2.WriteReadData(0xFF))==0xFF)
    {
        Retry++;	    
        if(Retry>200)break; 
    }  	  
    //������Ӧֵ
    return r1;
}

/*******************************************************************************
* ������	: SD_Idle_Sta
* ����	: ��SD�����õ�����ģʽ
* �������  : ��
* ���ز���  : SD�����ص���Ӧ
*******************************************************************************/
uint8_t SD_Idle_Sta(void)
{
	uint16_t kk;
	uint8_t retry;
    for(kk=0;kk<0xf00;kk++);     	//����ʱ���ȴ�SD���ϵ����
    								//�Ȳ���>74�����壬��SD���Լ���ʼ�����
    for(kk=0;kk<10;kk++)
    	OSBsp.Device.Spi2.WriteReadData(0xFF);

    //-----------------SD����λ��idle��ʼ-----------------
    //ѭ����������CMD0��ֱ��SD������0x01,����IDLE״̬
    //��ʱ��ֱ���˳�
    retry = 0;
    do
    {	   
        //����CMD0����SD������IDLE״̬
        kk = SD_SendCommand(CMD0, 0, 0x95);
        retry++;
    }while((kk!=0x01)&&(retry<200));

    //����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ��
    if(retry==200)
    	return 1; 		//ʧ��

	return 0;			//�ɹ�
}														    

/*******************************************************************************
* ������	: g_Device_SD_Init
* ����	: SD����ʼ��
* �������  : ��
* ���ز���  : SD�����ص���Ӧ
*******************************************************************************/
uint8_t g_Device_SD_Init(void)
{								 
	uint8_t r1;     		// ���SD���ķ���ֵ
	uint16_t retry;  		// �������г�ʱ����
	uint8_t buff[6];

	g_Device_SPI2_Init(); 
	SD_CS_High();
    if(SD_Idle_Sta())
    	return 1;			//��ʱ����1 ���õ�idle ģʽʧ��
    //-----------------SD����λ��idle����-----------------	 
    //��ȡ��Ƭ��SD�汾��Ϣ
// 	SD_CS_Low();
    SD_CS_Low();
	r1 = SD_SendCommand_NoDeassert(8, 0x1aa,0x87);	     
    //�����Ƭ�汾��Ϣ��v1.0�汾�ģ���r1=0x05����������³�ʼ��
    if(r1 == 0x05)
    {
        //���ÿ�����ΪSDV1.0����������⵽ΪMMC�������޸�ΪMMC
        SD_Type = SD_TYPE_V1;	   
        //�����V1.0����CMD8ָ���û�к�������
        //Ƭѡ�øߣ�������������
        SD_CS_High();
        //�෢8��CLK����SD������������
        OSBsp.Device.Spi2.WriteReadData(0xFF);	  
        //-----------------SD����MMC����ʼ����ʼ-----------------	 
        //������ʼ��ָ��CMD55+ACMD41
        // �����Ӧ��˵����SD�����ҳ�ʼ�����
        // û�л�Ӧ��˵����MMC�������������Ӧ��ʼ��
        retry = 0;
        do
        {
            //�ȷ�CMD55��Ӧ����0x01���������
            r1 = SD_SendCommand(CMD55, 0, 0);
            if(r1 == 0XFF)return r1;//ֻҪ����0xff,�ͽ��ŷ���	  
            //�õ���ȷ��Ӧ�󣬷�ACMD41��Ӧ�õ�����ֵ0x00����������200��
            r1 = SD_SendCommand(ACMD41, 0, 0);
            retry++;
        }while((r1!=0x00) && (retry<400));
        // �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
        // ���л�Ӧ����SD����û�л�Ӧ����MMC��	  
        //----------MMC�������ʼ��������ʼ------------
        if(retry==400)
        {
            retry = 0;
            //����MMC����ʼ�����û�в��ԣ�
            do
            {
                r1 = SD_SendCommand(1,0,0);
                retry++;
            }while((r1!=0x00)&& (retry<400));
            if(retry==400)return 1;   //MMC����ʼ����ʱ		    
            //д�뿨����
            SD_Type = SD_TYPE_MMC;
        }
        //----------MMC�������ʼ����������------------
     
          
		OSBsp.Device.Spi2.WriteReadData(0xFF);	 
        //��ֹCRCУ��	   
		r1 = SD_SendCommand(CMD59, 0, 0x95);
        if(r1 != 0x00)return r1;  //������󣬷���r1   	   
        //����Sector Size
        r1 = SD_SendCommand(CMD16, 512, 0x95);
        if(r1 != 0x00)return r1;  //������󣬷���r1
        //-----------------SD����MMC����ʼ������-----------------

    }//SD��ΪV1.0�汾�ĳ�ʼ������	 
     //������V2.0���ĳ�ʼ��
     //������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
    else if(r1 == 0x01)
    {
        //V2.0�Ŀ���CMD8�����ᴫ��4�ֽڵ����ݣ�Ҫ�����ٽ���������
        buff[0] = OSBsp.Device.Spi2.WriteReadData(0xFF);  //should be 0x00
        buff[1] = OSBsp.Device.Spi2.WriteReadData(0xFF);  //should be 0x00
        buff[2] = OSBsp.Device.Spi2.WriteReadData(0xFF);  //should be 0x01
        buff[3] = OSBsp.Device.Spi2.WriteReadData(0xFF);  //should be 0xAA	    
        SD_CS_High();
        OSBsp.Device.Spi2.WriteReadData(0xFF);//the next 8 clocks			 
        //�жϸÿ��Ƿ�֧��2.7V-3.6V�ĵ�ѹ��Χ
        //if(buff[2]==0x01 && buff[3]==0xAA) //���жϣ�����֧�ֵĿ�����
        {	  
            retry = 0;
            //������ʼ��ָ��CMD55+ACMD41
    		do
    		{
    			r1 = SD_SendCommand(CMD55, 0, 0);
    			if(r1!=0x01)return r1;	   
    			r1 = SD_SendCommand(ACMD41, 0x40000000, 0);
                if(retry>200)return r1;  //��ʱ�򷵻�r1״̬  
            }while(r1!=0);		  
            //��ʼ��ָ�����ɣ���������ȡOCR��Ϣ		   
            //-----------����SD2.0���汾��ʼ-----------
            r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
            if(r1!=0x00)
			{
				SD_CS_High();//�ͷ�SDƬѡ�ź�
				return r1;  //�������û�з�����ȷӦ��ֱ���˳�������Ӧ��	 
			}//��OCRָ����󣬽�������4�ֽڵ�OCR��Ϣ
            buff[0] = OSBsp.Device.Spi2.WriteReadData(0xFF);
            buff[1] = OSBsp.Device.Spi2.WriteReadData(0xFF); 
            buff[2] = OSBsp.Device.Spi2.WriteReadData(0xFF);
            buff[3] = OSBsp.Device.Spi2.WriteReadData(0xFF);		 
            //OCR������ɣ�Ƭѡ�ø�
            SD_CS_High();
            OSBsp.Device.Spi2.WriteReadData(0xFF);	   
            //�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
            //���CCS=1��SDHC   CCS=0��SD2.0
            if(buff[0]&0x40)SD_Type = SD_TYPE_V2HC;    //���CCS	 
            else SD_Type = SD_TYPE_V2;	    
            //-----------����SD2.0���汾����----------- 
            
        }	    
    }
    return r1;
}	 																			   

/*******************************************************************************
* ������	: SD_ReceiveData
* ����	: ��SD���ж���ָ�����ȵ����ݣ������ڸ���λ��
* �������  : *data(��Ŷ������ݵ��ڴ�>len)
			len(���ݳ��ȣ�
			release(������ɺ��Ƿ��ͷ�����CS�ø� 0�����ͷ� 1���ͷţ�
* ���ز���  : 0��NO_ERR
*******************************************************************************/
uint8_t SD_ReceiveData(uint8_t *data, uint16_t len, uint8_t release)
{
    // ����һ�δ���
    SD_CS_Low();
	if(SD_GetResponse(0xFE))//�ȴ�SD������������ʼ����0xFE
	{	  
		SD_CS_High();
		return 1;
	}
    while(len--)//��ʼ��������
    {
        *data=OSBsp.Device.Spi2.WriteReadData(0xFF);
        data++;
    }
    //������2��αCRC��dummy CRC��
    OSBsp.Device.Spi2.WriteReadData(0xFF);
    OSBsp.Device.Spi2.WriteReadData(0xFF);
    if(release==RELEASE)//�����ͷ����ߣ���CS�ø�
    {
        SD_CS_High();//�������
        OSBsp.Device.Spi2.WriteReadData(0xFF);
    }

    return 0;
}

/*******************************************************************************
* ������	: SD_GetCID
* ����	: ��ȡSD����CID��Ϣ��������������Ϣ
* �������  : *cid_data(���CID���ڴ棬����16Byte��
* ���ز���  : 0��NO_ERR
			1��TIME_OUT
			other��������Ϣ
*******************************************************************************/
uint8_t SD_GetCID(uint8_t *cid_data)
{
	uint8_t r1;
    //��CMD10�����CID
    r1 = SD_SendCommand(CMD10,0,0xFF);
    if(r1 != 0x00)
    	return r1;  						//û������ȷӦ�����˳�������

    SD_ReceiveData(cid_data,16,RELEASE);	//����16���ֽڵ�����

    return 0;
}

/*******************************************************************************
* ������	: SD_GetCSD
* ����	: ��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
* �������  : *cid_data(���CID���ڴ棬����16Byte��
* ���ز���  : 0��NO_ERR
			1��TIME_OUT
			other��������Ϣ
*******************************************************************************/
uint8_t SD_GetCSD(uint8_t *csd_data)
{
	uint8_t r1;
    r1=SD_SendCommand(CMD9,0,0xFF);				//��CMD9�����CSD
    if(r1)
    	return r1; 							 	//û������ȷӦ�����˳�������

    SD_ReceiveData(csd_data, 16, RELEASE);		//����16���ֽڵ�����

    return 0;
}  

/*******************************************************************************
* ������	: SD_GetCapacity
* ����	: ��ȡSD�����������ֽڣ�
* �������  : ��
* ���ز���  : SD������
*******************************************************************************/
uint32_t SD_GetCapacity(void)
{
	uint8_t csd[16];
	uint32_t Capacity;
	uint8_t r1;
	uint16_t kk;
	uint16_t temp;

	//ȡCSD��Ϣ������ڼ����������0
    if(SD_GetCSD(csd)!=0)
    	return 0;
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)
    {									  
	    Capacity=((uint32_t)csd[8])<<8;
		Capacity+=(uint32_t)csd[9]+1;
        Capacity = (Capacity)*1024;//�õ�������
		Capacity*=512;//�õ��ֽ���			   
    }
    else
    {		    
    	kk = csd[6]&0x03;
    	kk <<=8;
    	kk += csd[7];
    	kk <<=2;
    	kk += ((csd[8]&0xc0)>>6);
        //C_SIZE_MULT
    	r1 = csd[9]&0x03;
    	r1<<=1;
    	r1 += ((csd[10]&0x80)>>7);	 
    	r1+=2;//BLOCKNR
    	temp = 1;
    	while(r1)
    	{
    		temp *= 2;
    		r1 --;
    	}
    	Capacity = ((uint32_t)(kk+1))*((uint32_t)temp);
        // READ_BL_LEN
    	kk = csd[5]&0x0f;
        //BLOCK_LEN
    	temp = 1;
    	while(kk)
    	{
    		temp*=2;
    		kk--;
    	}
        //The final result
    	Capacity *= (uint32_t)temp;   //�ֽ�Ϊ��λ
    }

    return (uint32_t)Capacity;
}	    																			    

/*******************************************************************************
* ������	: SD_ReadSingleBlock
* ����	: ��SD����һ��block
* �������  : sector ȡ��ַ��sectorֵ����������ַ��
			*buffer ���ݴ洢��ַ����С����512byte��
* ���ز���  : 0�� �ɹ�
	     	other��ʧ��
*******************************************************************************/
uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer)
{
	uint8_t r1;
  		   
    //�������SDHC����������sector��ַ������ת����byte��ַ
    if(SD_Type!=SD_TYPE_V2HC)
    {
        sector = sector<<9;
    } 
	r1 = SD_SendCommand(CMD17, sector, 0);//������	 		    
	if(r1 != 0x00)
		return r1;

	r1 = SD_ReceiveData(buffer, 512, RELEASE);

	if(r1 != 0)
		return r1;   //�����ݳ�����
    else
    	return 0;
}


////////////////////////////����2������ΪUSB��д����Ҫ��/////////////////////////
//����SD���Ŀ��С	 				   
#define BLOCK_SIZE 512 


/*******************************************************************************
* ������	: MSD_WriteBuffer
* ����	: д��MSD/SD����
* �������  : pBuffer:���ݴ����
			ReadAddr:д����׵�ַ
			NumByteToRead:Ҫд����ֽ���
* ���ز���  : 0�� д�����
	     	other��ʧ��
*******************************************************************************/
uint8_t MSD_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
	uint32_t i,NbrOfBlock = 0, Offset = 0;
	uint32_t sector;
	uint8_t r1;

   	NbrOfBlock = NumByteToWrite / BLOCK_SIZE;//�õ�Ҫд��Ŀ����Ŀ

    SD_CS_Low();
	while (NbrOfBlock--)			//д��һ������
	{
		sector=WriteAddr+Offset;
		if(SD_Type==SD_TYPE_V2HC)
			sector>>=9;				//ִ������ͨ�����෴�Ĳ���
		r1=SD_SendCommand_NoDeassert(CMD24,sector,0xff);//д����   
 		if(r1)
		{
			SD_CS_High();

			return 1;//Ӧ����ȷ��ֱ�ӷ��� 	   
	    }

	    OSBsp.Device.Spi2.WriteReadData(0xFE);//����ʼ����0xFE   
	    //��һ��sector������
	    for(i=0;i<512;i++)
	    	OSBsp.Device.Spi2.WriteReadData(*pBuffer++);
	    //��2��Byte��dummy CRC
	    OSBsp.Device.Spi2.WriteReadData(0xff);
	    OSBsp.Device.Spi2.WriteReadData(0xff); 
 		if(SD_WaitDataReady())//�ȴ�SD������д�����
		{
			SD_CS_High();
			return 2;    
		}
		Offset += 512;	   
	}	    
    //д����ɣ�Ƭѡ��1
    SD_CS_High();
    OSBsp.Device.Spi2.WriteReadData(0xff);	 

    return 0;
}

/*******************************************************************************
* ������	: MSD_ReadBuffer
* ����	: ��ȡMSD/SD����
* �������  : pBuffer:���ݴ����
			ReadAddr:��ȡ���׵�ַ
			NumByteToRead:Ҫ��ȡ���ֽ���
* ���ز���  : 0�� ���
	     	other��ʧ��
*******************************************************************************/
uint8_t MSD_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	uint32_t NbrOfBlock=0,Offset=0;
	uint32_t sector=0;
	uint8_t r1=0;
  	NbrOfBlock=NumByteToRead/BLOCK_SIZE;	  
    SD_CS_Low();

	while (NbrOfBlock --)
	{	
		sector=ReadAddr+Offset;
		if(SD_Type==SD_TYPE_V2HC)
			sector>>=9;										//ִ������ͨ�����෴�Ĳ���
		r1=SD_SendCommand_NoDeassert(CMD17,sector,0xff);	//������
		if(r1)//����ʹ���
		{
    		SD_CS_High();
			return r1;
		}	   							  
		r1=SD_ReceiveData(pBuffer,512,RELEASE);		 
		if(r1)//��������
		{
    		SD_CS_High();
			return r1;
		}
		pBuffer+=512;	 					    
	  	Offset+=512;				 	 
	}	 	 
    SD_CS_High();
    OSBsp.Device.Spi2.WriteReadData(0xff);

    return 0;
}

/*******************************************************************************
* ������	: SD_WriteSingleBlock
* ����	: д��SD����һ��block(δʵ�ʲ��Թ�)
* �������  : sector ������ַ��sectorֵ����������ַ��
			*buffer ���ݴ洢��ַ����С����512byte��
* ���ز���  : 0�� ���
	     	other��ʧ��
*******************************************************************************/
uint8_t SD_WriteSingleBlock(uint32_t sector, const uint8_t *data)
{
	uint8_t r1;
	uint16_t kk;
	uint16_t retry;


    if(SD_Type != SD_TYPE_V2HC)
    {
        sector = sector<<9;
    }   
    r1 = SD_SendCommand(CMD24, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1;  //Ӧ����ȷ��ֱ�ӷ���
    }
    
    //��ʼ׼�����ݴ���
    SD_CS_Low();
    //�ȷ�3�������ݣ��ȴ�SD��׼����
    OSBsp.Device.Spi2.WriteReadData(0xff);
    OSBsp.Device.Spi2.WriteReadData(0xff);
    OSBsp.Device.Spi2.WriteReadData(0xff);
    //����ʼ����0xFE
    OSBsp.Device.Spi2.WriteReadData(0xFE);

    //��һ��sector������
    for(kk=0;kk<BLOCK_SIZE;kk++)
    {
        OSBsp.Device.Spi2.WriteReadData(*data++);
    }
    //��2��Byte��dummy CRC
    OSBsp.Device.Spi2.WriteReadData(0xff);
    OSBsp.Device.Spi2.WriteReadData(0xff);
    
    //�ȴ�SD��Ӧ��
    r1 = OSBsp.Device.Spi2.WriteReadData(0xff);
    if((r1&0x1F)!=0x05)
    {
        SD_CS_High();
        return r1;
    }
    
    //�ȴ��������
    retry = 0;
    while(!OSBsp.Device.Spi2.WriteReadData(0xff))
    {
        retry++;
        if(retry>0xfffe)        //�����ʱ��д��û����ɣ������˳�
        {
            SD_CS_High();
            return 1;           //д�볬ʱ����1
        }
    }	    
    //д����ɣ�Ƭѡ��1
    SD_CS_High();
    OSBsp.Device.Spi2.WriteReadData(0xff);

    return 0;
}				           

/*******************************************************************************
* ������	: SD_ReadMultiBlock
* ����	: ��SD���Ķ��block(ʵ�ʲ��Թ�)
* �������  : sector ������ַ��sectorֵ����������ַ��
			*buffer ���ݴ洢��ַ����С����512byte��
			count ������count��block
* ���ز���  : 0�� ���
	     	other��ʧ��
*******************************************************************************/
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count)
{
	uint8_t r1;
    //SPIx_SetSpeed(SPI_SPEED_HIGH);//����Ϊ����ģʽ  
 	//�������SDHC����sector��ַת��byte��ַ
    if(SD_Type!=SD_TYPE_V2HC)sector = sector<<9;  
    //SD_WaitDataReady();
    //�����������
	r1 = SD_SendCommand(CMD18, sector, 0);//������
	if(r1 != 0x00)
		return r1;
    do//��ʼ��������
    {
        if(SD_ReceiveData(buffer, 512, NO_RELEASE) != 0x00)
        	break;

        buffer += 512;
    } while(--count);		 
    //ȫ��������ϣ�����ֹͣ����
    SD_SendCommand(CMD12, 0, 0);
    //�ͷ�����
    SD_CS_High();
    OSBsp.Device.Spi2.WriteReadData(0xFF);    

    if(count != 0)
    	return count;   //���û�д��꣬����ʣ�����
    else
    	return 0;
}											  

/*******************************************************************************
* ������	: SD_ReadMultiBlock
* ����	: д��SD����N��block(δʵ�ʲ��Թ�)
* �������  : sector ������ַ��sectorֵ����������ַ��
			*buffer ���ݴ洢��ַ����С����512byte��
			count д���block��Ŀ
* ���ز���  : 0�� ���
	     	other��ʧ��
*******************************************************************************/
uint8_t SD_WriteMultiBlock(uint32_t sector, const uint8_t *data, uint8_t count)
{
	uint8_t r1;
	uint16_t kk;
    //SPIx_SetSpeed(SPI_SPEED_HIGH);//����Ϊ����ģʽ	 
    if(SD_Type != SD_TYPE_V2HC)sector = sector<<9;//�������SDHC����������sector��ַ������ת����byte��ַ  
    if(SD_Type != SD_TYPE_MMC) r1 = SD_SendCommand(ACMD23, count, 0x00);//���Ŀ�꿨����MMC��������ACMD23ָ��ʹ��Ԥ����   
    r1 = SD_SendCommand(CMD25, sector, 0x00);//�����д��ָ��
    if(r1 != 0x00)return r1;  //Ӧ����ȷ��ֱ�ӷ���	 
    SD_CS_Low();//��ʼ׼�����ݴ���
    OSBsp.Device.Spi2.WriteReadData(0xff);//�ȷ�3�������ݣ��ȴ�SD��׼����
    OSBsp.Device.Spi2.WriteReadData(0xff);   
    //--------������N��sectorд���ѭ������
    do
    {
        //����ʼ����0xFC �����Ƕ��д��
        OSBsp.Device.Spi2.WriteReadData(0xFC);	  
        //��һ��sector������
        for(kk=0;kk<BLOCK_SIZE;kk++)
        {
            OSBsp.Device.Spi2.WriteReadData(*data++);
        }
        //��2��Byte��dummy CRC
        OSBsp.Device.Spi2.WriteReadData(0xff);
        OSBsp.Device.Spi2.WriteReadData(0xff);
        
        //�ȴ�SD��Ӧ��
        r1 = OSBsp.Device.Spi2.WriteReadData(0xff);
        if((r1&0x1F)!=0x05)
        {
            SD_CS_High();    //���Ӧ��Ϊ����������������ֱ���˳�
            return r1;
        }		   
        //�ȴ�SD��д�����
        if(SD_WaitDataReady()==1)
        {
            SD_CS_High();    //�ȴ�SD��д����ɳ�ʱ��ֱ���˳�����
            return 1;
        }	   
    }while(--count);//��sector���ݴ������  
    //��������������0xFD
    r1 = OSBsp.Device.Spi2.WriteReadData(0xFD);
    if(r1==0x00)
    {
        count =  0xfe;
    }		   
    if(SD_WaitDataReady()) //�ȴ�׼����
	{
		SD_CS_High();
		return 1;  
	}
    //д����ɣ�Ƭѡ��1
    SD_CS_High();
    OSBsp.Device.Spi2.WriteReadData(0xff);  

    return count;   //����countֵ�����д����count=0������count=1
}						  					  
//
//����:
//����ֵ:0�� �ɹ�
//       other��ʧ��
/*******************************************************************************
* ������	: SD_Read_Bytes
* ����	: ��ָ������,��offset��ʼ����bytes���ֽ�
* �������  : sector ������ַ��sectorֵ����������ַ��
			*buf   ���ݴ洢��ַ����С<=512byte��
			offset �����������ƫ����
			bytes  Ҫ�������ֽ���
* ���ز���  : 0�� ���
	     	other��ʧ��
*******************************************************************************/
uint8_t SD_Read_Bytes(uint32_t address,uint8_t *buf,uint16_t offset,uint16_t bytes)
{
	uint8_t r1;
	uint16_t kk=0;
    r1=SD_SendCommand(CMD17,address<<9,0);//���Ͷ���������      
    if(r1)
	return r1;  			//Ӧ����ȷ��ֱ�ӷ���

	SD_CS_Low();			//ѡ��SD��
	if(SD_GetResponse(0xFE))//�ȴ�SD������������ʼ����0xFE
	{
		SD_CS_High(); 		//�ر�SD��
	return 1;				//��ȡʧ��
	}	 
	for(kk=0;kk<offset;kk++)
		OSBsp.Device.Spi2.WriteReadData(0xff);		//����offsetλ
    for(;kk<offset+bytes;kk++)
    	*buf++=OSBsp.Device.Spi2.WriteReadData(0xff);//��ȡ��������
    for(;kk<512;kk++)
    	OSBsp.Device.Spi2.WriteReadData(0xff); 	 	//����ʣ���ֽ�

OSBsp.Device.Spi2.WriteReadData(0xff);				//����αCRC��
    OSBsp.Device.Spi2.WriteReadData(0xff);  
    SD_CS_High();						//�ر�SD��

	return 0;
}




