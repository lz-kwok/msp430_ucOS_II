#ifndef   G_DEVICERTC_H
#define   G_DEVICERTC_H

#include <stdint.h>

/*DS1302�ܽŶ���*/
/*DS1302��λ��*/
#define RESET_CLR   	P1OUT &= ~BIT2            //��ƽ�õ�
#define RESET_SET	    P1OUT |=  BIT2            //��ƽ�ø�
#define RESET_IN	    P1DIR &= ~BIT2            //��������
#define RESET_OUT	    P1DIR |=  BIT2            //�������
/*DS1302˫������*/
#define IO_CLR	        P1OUT &= ~BIT1            //��ƽ�õ�
#define IO_SET	        P1OUT |=  BIT1            //��ƽ�ø�
#define IO_R	        P1IN & BIT1               //��ƽ��ȡ
#define IO_IN	        P1DIR &= ~BIT1            //��������
#define IO_OUT	        P1DIR |=  BIT1            //�������
/*DS1302ʱ���ź�*/
#define SCK_CLR	        P1OUT &= ~BIT0            //ʱ���ź�
#define SCK_SET	        P1OUT |=  BIT0            //��ƽ�ø�
#define SCK_IN	        P1DIR &= ~BIT0            //��������
#define SCK_OUT	        P1DIR |=  BIT0            //�������
/*DS1302��������ص�ַ*/
#define DS1302_sec_add		    0x80		      //�����ݵ�ַ
#define DS1302_min_add		    0x82		      //�����ݵ�ַ
#define DS1302_hr_add		    0x84		      //ʱ���ݵ�ַ
#define DS1302_date_add		    0x86		      //�����ݵ�ַ
#define DS1302_month_add	    0x88		      //�����ݵ�ַ
#define DS1302_day_add		    0x8a		      //�������ݵ�ַ
#define DS1302_year_add		    0x8c		      //�����ݵ�ַ
#define DS1302_clock_min_add	0xc3		      //�����ݵ�ַ_����
#define DS1302_clock_hr_add	    0xc5		      //ʱ���ݵ�ַ_����
#define DS1302_FLASH            0XC7
#define DS1302_control_add	    0x8e		      //�������ݵ�ַ
#define DS1302_charger_add	    0x90
#define DS1302_clkburst_add	    0xbe


extern uint8_t time_buf[8];
extern uint8_t time_string_buf[16];
extern uint8_t clock_buf[2];
extern uint8_t clock_flag;


void DS1302_write_byte(uint8_t addr, uint8_t d);
uint8_t DS1302_read_byte(uint8_t addr);
void DS1302_write_time(void);
void DS1302_read_time(void);
void DS1302_init(void);
void Create_TimeData(uint8_t *p1,uint8_t *p2);
void Create_TimeString(uint8_t *p1,uint8_t *p2);

void g_Device_RTC_Init(void);
void Write_info_RTC(uint8_t *time);
void Read_info_RTC(uint8_t *time);


#endif

