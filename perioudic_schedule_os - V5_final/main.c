
#include "osKernel.h"
#include "STM32F4_RTOS_BSP.h"


#define	QUANTA	10


uint32_t count0,count1,count2,periodic_count1,periodic_count2,periodic_count3,Interrupt_count;

void Task0(void)
{
	 while(1){
	 count0++;
	  
		 
	 }
}

void Task1(void)
{
	 while(1){
	 count1++;
	

	 }
}
void periodic_Task1()
{
	periodic_count1++;
	BSP_LED_redOn();
		 BSP_LED_blueOff();
		 BSP_LED_greenOff();
	for(int i=0;i<1000;i++)
	{	
	for(int j=0;j<2000;j++);
	}
}

void periodic_Task2()
{
	periodic_count2++;
	BSP_LED_redOff();
		 BSP_LED_blueOn();
		 BSP_LED_greenOff();
	for(int i=0;i<1000;i++)
	{	
	for(int j=0;j<2000;j++);
	}
}

void periodic_Task3()
{
	periodic_count3++;
		BSP_LED_redOff();
		 BSP_LED_blueOff();
		 BSP_LED_greenOn();
	for(int i=0;i<1000;i++)
	{	
	for(int j=0;j<2000;j++);
	}
}
	
	
void Task2(void)
{
	 while(1){
	  count2++;
		 

	 }
}
int main(void)
{
	BSP_LED_Init();
	//TIMER2_Init();
	osKernelInit();
	osKernelAddThreads(&Task0,&Task1,&Task2);
	osKernelAddPeriodicThreads(&periodic_Task1,100);
	osKernelAddPeriodicThreads(&periodic_Task2,200);
	osKernelAddPeriodicThreads(&periodic_Task3,400);
	osKernelLaunch(QUANTA);
	
}


