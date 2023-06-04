/*
   -schedular
	 -Thread Control Block
	 -Stack
*/

#include "osKernel.h"
#define NUM_OF_THREADS 3

#define NUM_OF_PERIODIC_TASK  5
#define STACKSIZE 100
typedef void (*TaskT)(void);
#define NULL (void *)0
	
#define BUS_FREQ		16000000
#define PERIOUD   100
uint32_t MILLIS_PRESCALER;
/*static varible to increament en new periodic thread is added*/
static uint32_t NumPeriodicThread=0;
void osSchedularLaunch(void);
void Periodic_Events_Excute(void);
void OsPeriodicTask_Init(void (*Ptask)(void),uint32_t Ferq,uint32_t Priority);
/*Systick priority register*/
#define SYSPRI3         (*((volatile uint32_t *)0xE000ED20))
#define INTCTRL         (*((volatile uint32_t *)0xE000ED04))
#define NVIC_INT_CTRL_REG         (*((volatile uint32_t *)0xE000ED04))
typedef struct
{
	TaskT task;
	uint32_t Period;
	uint32_t TimeLeft;
}periodicTask;

static periodicTask periodicTasks[NUM_OF_PERIODIC_TASK];
static uint32_t TimeMSec;
static uint32_t MaxPeriod;

struct tcb{
	
  int32_t *stackPt;
	struct tcb *nextPt;
};

typedef struct tcb tcbType;
tcbType tcbs[NUM_OF_THREADS];
tcbType *currentPt;

int32_t TCB_STACK[NUM_OF_THREADS][STACKSIZE];

void osKernelStackInit(int i){
   tcbs[i].stackPt =  &TCB_STACK[i][STACKSIZE -16];//StackPointer
	 TCB_STACK[i][STACKSIZE -1] =  0x01000000;
	
	TCB_STACK[i][STACKSIZE-3] = 0x14141414;  //R14
	TCB_STACK[i][STACKSIZE-4] = 0x12121212;  //R12
	TCB_STACK[i][STACKSIZE-5] = 0x03030303;  //R3
	TCB_STACK[i][STACKSIZE-6] = 0x02020202;  //R2
	TCB_STACK[i][STACKSIZE-7] = 0x01010101;  //R1
	TCB_STACK[i][STACKSIZE-8] = 0x00000000;  //R0
	  /*We have to save manually*/	
	TCB_STACK[i][STACKSIZE-9] = 0x11111111;  //R11
	TCB_STACK[i][STACKSIZE-10] = 0x10101010; //R10
	TCB_STACK[i][STACKSIZE-11] = 0x09090909; //R9
	TCB_STACK[i][STACKSIZE-12] = 0x08080808; //R8
	TCB_STACK[i][STACKSIZE-13] = 0x07070707; //R7
	TCB_STACK[i][STACKSIZE-14] = 0x06060606; //R6
	TCB_STACK[i][STACKSIZE-15] = 0x05050505; //R5
	TCB_STACK[i][STACKSIZE-16] = 0x04040404; //R4
}


uint8_t osKernelAddThreads(void(*task0)(void), void(*task1)(void),void(*task2)(void))
{
	__disable_irq();
	
	tcbs[0].nextPt = &tcbs[1];
	tcbs[1].nextPt = &tcbs[2];
	tcbs[2].nextPt = &tcbs[0];
	
	
	osKernelStackInit(0);
	TCB_STACK[0][STACKSIZE-2] = (int32_t)(task0);
	
  osKernelStackInit(1);
	TCB_STACK[1][STACKSIZE-2] = (int32_t)(task1);
	
  osKernelStackInit(2);
	TCB_STACK[2][STACKSIZE-2] = (int32_t)(task2);
	
	currentPt =&tcbs[0];
	
	__enable_irq();
	
		return 1;

	
}



void osKernelInit(void)
{
	MILLIS_PRESCALER =  (BUS_FREQ/1000);
	OsPeriodicTask_Init(Periodic_Events_Excute,1000,6);
}


void osKernelLaunch(uint32_t quanta)
{
	 SysTick->CTRL =0;
	 SysTick->VAL  =0;
	 SysTick->LOAD =  (quanta*MILLIS_PRESCALER)-1;
	
	 SYSPRI3 =(SYSPRI3&0x00FFFFFF)|0xE0000000; // priority 7
	 SysTick->CTRL =  0x00000007;
	
	osSchedularLaunch();
	

}

void osThreadYield(void){

 SysTick->VAL =0;
 INTCTRL = 0x04000000; // trigger SysTick

}
void SysTick_Handler(void){
	//trigger pendSV
	INTCTRL=0x10000000;
	
	
}

uint32_t Period_Tick;
void osSchedulerPeriodicRR(void)
{
	
	/*when periodic task is excuted go bac to round roin schedule*/
	currentPt=currentPt->nextPt;
}


/*funcation to use to add new periodic thread*/

uint8_t osKernelAddPeriodicThreads(void(*Ptask)(void),uint32_t period)
{
	/*check if can add new thread or not ->the maximam task can os handle can be change by edit macro NUM_OF_PERIODIC_TASK */
	if(NumPeriodicThread==NUM_OF_PERIODIC_TASK ||period==0)
	{
		return 0;
	}else
	{
		periodicTasks[NumPeriodicThread].task=Ptask;
		periodicTasks[NumPeriodicThread].Period=period;
		periodicTasks[NumPeriodicThread].TimeLeft=period-1;
		NumPeriodicThread++;
		return 1;
	}
	
}

void Periodic_Events_Excute(void)
{
	int i;
	for(i=0;i<NumPeriodicThread;i++)
	{
		/*the time left for a periodic task to execute is zero.Meaning is it time to execute*/
		if(periodicTasks[i].TimeLeft==0)
		{
				periodicTasks[i].task();
				periodicTasks[i].TimeLeft=periodicTasks[i].Period-1;
		}else
		{
			periodicTasks[i].TimeLeft--;
			
		}
	}
}
void (*PerioudicTaskInit)(void);
void OsPeriodicTask_Init(void (*Ptask)(void),uint32_t Ferq,uint32_t Priority)
{
	__disable_irq();
	PerioudicTaskInit=Ptask;
	RCC->APB1ENR |= 0x01;  /*Enable TIM2 clock*/
	TIM2->PSC = 16 -1;  /* 16 000 000 /16 = 1000 000*/
	TIM2->ARR = (1000000/Ferq) -1;
	TIM2->CR1 =1;  /*Counter enable*/
	TIM2->DIER |=1; /*Enable Interrupt*/
	NVIC_SetPriority(TIM2_IRQn,Priority);
	NVIC_EnableIRQ(TIM2_IRQn);
	
}


void TIM2_IRQHandler()
{
	TIM2->SR=0;
	(*PerioudicTaskInit)();
}