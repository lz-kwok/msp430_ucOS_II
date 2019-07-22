#ifndef __HAL_LAYER_API_H
#define __HAL_LAYER_API_H

#include  <ucos_ii.h>
#include  <cpu.h>
#include  <lib_def.h>
#include  <app_cfg.h>
#include  <bsp.h>

/*************************************************************************
* MACROS
**************************************************************************/
#define hal_Reboot()                  PMMCTL0 = PMMPW + PMMSWBOR + (PMMCTL0 & 0x0003)

#define null                          ((void *) 0)   //空指针
#define invalid                       0xFF

typedef OS_EVENT*                     Queue_t;
typedef OS_EVENT*                     Mutex_t;
typedef void                          (*message_free_cb)(void* content);




struct hal_message{
	int 	what;			/*msg type*/
	void* 	content;		/*msg content*/
	message_free_cb freecb; /* free content func */
};

/*Bit Operation Function*/
#define hal_SetBit(data, offset)      data |= 1 << offset      //置位某位为1
#define hal_ResetBit(data, offset)    data &= ~(1 << offset)   //复位某位为0
#define hal_GetBit(data, offset)      ((data >> offset) &0x01) //获取某位

#define hal_Delay_us(x)                __delay_cycles((long)(BSP_CPU_CLK_FREQ*(long)x/1000000.0))
#define hal_Delay_ms(x)                __delay_cycles((long)(BSP_CPU_CLK_FREQ*(long)x/1000.0))
#define hal_Delay_sec(x)               __delay_cycles((long)(BSP_CPU_CLK_FREQ*(long)x))


#define PRODUCT_NAMES_LEN             (32)


void *Hal_Malloc(int size);
void *Hal_Calloc(int count, int size);
void Hal_Free(void *ptr);

int Hal_ThreadCreate(void *func, void *funcname,void *TaskStk, int stack_size,int priority);
int Hal_ThreadDestory(int priority);
Queue_t Hal_QueueCreate(void **start,int size);
void Hal_QueueDestory(Queue_t queue);
int Hal_QueueSend(Queue_t queue, struct hal_message* msg, int timeout);
int Hal_QueueRecv(Queue_t queue, struct hal_message* msg, int timeout);
Mutex_t Hal_MutexCreate(int priority);
void Hal_MutexDestory(Mutex_t mutex);
void Hal_MutexLock(Mutex_t mutex);
void Hal_MutexUnlock(Mutex_t mutex);





#endif /* __hal_UCS */
