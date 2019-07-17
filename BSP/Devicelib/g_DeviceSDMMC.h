//*************************************************************************************
//MMC/SD����дͷ�ļ�mmc.h
//������SD����Ӳ�����ӡ��궨����MMC/SD�������������д�������������ش���(�ɹ����롢�������)
//����Ӳ�����Ӳ�ͬ�����Ը��Ķ˿�
//���Ի�����EW430 V5.30
//���ߣ����� www.avrgcc.com
//ʱ�䣺2011.09.19
// *************************************************************************************
#ifndef G_DEVICESDMMC_H
#define G_DEVICESDMMC_H


/**********************************************************************
              MMC/SD��Ӳ������IO�ڷ������
***********************************************************************/

#define HIGH(a) ((a>>8)&0xFF)                     //��ȡһ�������ݵĸ��ֽڲ���
#define LOW(a)  (a&0xFF)                          //��ȡһ�������ݵĵ��ֽڲ���


#define SPI_RXC     (IFG1 & URXIFG0)
#define SPI_TXC     (IFG1 & UTXIFG0)

#define SPI_RX_COMPLETE (IFG1 & URXIFG0)
#define SPI_TX_READY    (IFG1 & UTXIFG0)
#define SPI_TX_DONE     while((U0TCTL & TXEPT)==0)


/**********************************************************************
             ���ݴ��俪ʼ��־����Ҫ�ڶ����ݺ�д����ǰ���м��
***********************************************************************/

#define MMC_START_DATA_BLOCK_TOKEN          0xfe   //���ݴ��俪ʼ��־λ����ʼ���������
#define MMC_START_DATA_MULTIPLE_BLOCK_READ  0xfe   //���ݴ��俪ʼ��־λ����ʼ��������
#define MMC_START_DATA_BLOCK_WRITE          0xfe   //���ݴ��俪ʼ��־λ����ʼ����д����
#define MMC_START_DATA_MULTIPLE_BLOCK_WRITE 0xfc   //���ݴ��俪ʼ��־λ����ʼ���д����
#define MMC_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xfd   //���ݴ���ֹͣ��־λ��ֹͣ���д����

// an affirmative R1 response (no errors)
#define MMC_R1_RESPONSE       0x00

/**********************************************************************
             ���سɹ���������
***********************************************************************/

#define MMC_SUCCESS           0x00           //ִ�гɹ�����
#define MMC_BLOCK_SET_ERROR   0x01           //��û����ȫ���뵽������
#define MMC_RESPONSE_ERROR    0x02
#define MMC_DATA_TOKEN_ERROR  0x03
#define MMC_INIT_ERROR        0x04
#define MMC_CRC_ERROR         0x10
#define MMC_WRITE_ERROR       0x11
#define MMC_OTHER_ERROR       0x12
#define MMC_TIMEOUT_ERROR     0xFF

/**********************************************************************
              MMC/SD������
***********************************************************************/

#define MMC_GO_IDLE_STATE          0x40     //CMD0 MMC/SD���ϵ��������ģʽ
#define MMC_SEND_OP_COND           0x41     //CMD1 MMC����ʼ��ʹ��CMD1�����������ж�SD������MMC��
#define MMC_CID                    0x48     //CMD8 SD���汾�ж���Ϣ��Ver1.x����Ver2.x�������CMD8��Ӧ������ΪVer1.x
#define MMC_READ_CSD               0x49     //CMD9 ��ȡ������Ϣ��Ver1.xֻ֧�����2GB��Ver2.x֧�ָ�������
#define MMC_SEND_CID               0x4a     //CMD10 MMC/SD��������Ϣ������ID���������ڵȵȣ����Ǳ���
#define MMC_STOP_TRANSMISSION      0x4c     //CMD12 ������ʱֹͣ����
#define MMC_SEND_STATUS            0x4d     //CMD13 ����״̬��ȡ����
#define MMC_SET_BLOCKLEN           0x50     //CMD16 ���ö�д��ĳ���
#define MMC_READ_SINGLE_BLOCK      0x51     //CMD17 ����������
#define MMC_READ_MULTIPLE_BLOCK    0x52     //CMD18 �����
#define MMC_CMD_WRITEBLOCK         0x54     //CMD20 
#define MMC_WRITE_BLOCK            0x58     //CMD24 д����
#define MMC_WRITE_MULTIPLE_BLOCK   0x59     //CMD25
#define MMC_WRITE_CSD              0x5b     //CMD27 дCSD�Ĵ���
#define MMC_SET_WRITE_PROT         0x5c     //CMD28
#define MMC_CLR_WRITE_PROT         0x5d     //CMD29
#define MMC_SEND_WRITE_PROT        0x5e     //CMD30
#define MMC_TAG_SECTOR_START       0x60     //CMD32 ������ʼ��ַ
#define MMC_TAG_SECTOR_END         0x61     //CMD33 ����������ַ
#define MMC_UNTAG_SECTOR           0x62     //CMD34
#define MMC_TAG_EREASE_GROUP_START 0x63     //CMD35
#define MMC_TAG_EREASE_GROUP_END   0x64     //CMD36
#define MMC_UNTAG_EREASE_GROUP     0x65     //CMD37
#define MMC_EREASE                 0x66     //CMD38 ��������
#define MMC_READ_OCR               0x67     //CMD39 ��OCR�Ĵ���
#define MMC_CRC_ON_OFF             0x68     //CMD40 �ر�CRCУ��

#define SD_APP_COND                0x77     //CMD55  SD����ʼ������дCMD55�����дAMD41����
#define SD_APP_OP_COND             0x69     //ACMD41 ��ʼ��SD����MMC����CMD1

/********************************************************************************
                    ����������
********************************************************************************/
//extern unsigned int BlockLength;     //MMC/SD�����С��CSD�Ĵ�����¼�˿����������Ĵ�С��һ��Ϊ512
//extern unsigned long numSector;	     //��SD����д�����ݵĵ�ַ��Ϊ������������һ������Ϊ512�ֽڵ�SD��ʵ�ʵ�ַΪnumSector*512�ֽ�


//void initSPI (void);
//unsigned char spiSendByte(const unsigned char data);
char mmc_GoIdle();
unsigned char mmcGetResponse(void);
unsigned char mmcGetXXResponse(const char resp);
unsigned char mmcCheckBusy(void);
void spiInit(void);
unsigned char spiSendByte(unsigned char data);
unsigned char mmcGoIdle();
unsigned char initMMC (void);
unsigned char mmcEreaseBlock(unsigned long Sector);
void mmcSendCmd (unsigned char cmd, unsigned long data, unsigned char crc);
unsigned char mmcSetBlockLength (unsigned int blocklength);
unsigned char mmcReadBlock(unsigned long address, unsigned int count, unsigned char *pBuffer);
#define mmcReadSector(sector, pBuffer) mmcReadBlock(sector*512ul, 512, pBuffer)  //����������������Ϊ�������������ݵ�ַ
unsigned char mmcWriteBlock(unsigned long address, unsigned int count, unsigned char *pBuffer);
#define mmcWriteSector(sector, pBuffer) mmcWriteBlock(sector*512ul, 512, pBuffer) //д��������������Ϊ����������д�����ݵ�ַ
unsigned char mmcReadRegister(unsigned char cmd_register, unsigned char length, unsigned char *pBuffer);
unsigned long MMC_ReadCardSize(void);


#endif
