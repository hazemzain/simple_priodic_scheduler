#ifndef __OS_KERNEL_H__
#define __OS_KERNEL_H__

#include <stdint.h>

#include "stm32f4xx.h"                  // Device header


void osKernelLaunch(uint32_t quanta);
void osKernelInit(void);
uint8_t osKernelAddThreads(void(*task0)(void), void(*task1)(void),void(*task2)(void));
void osThreadYield(void);
void periodic_Task(void);
void periodic_Task1(void);

void periodic_Task2(void);

uint8_t osKernelAddPeriodicThreads(void(*Ptask0)(void),uint32_t period0);

#endif