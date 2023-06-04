# simple_priodic_scheduler

I finished Implementation of simple periodic schedule by using stm32f429 discovery board .
✅The project constant of three tasks: 
⭐️Task1 turn on an LED B0 every 500 ms,
⭐️Task2 turn on an LED B1every 1000 ms,
 ⭐️Task3 turn on an LED B2 every 500 ms,
✅each task keeps a task's period, elapsed time, and a pointer to the task's function.
✅I move the Context Switching from SysTick Handler to the PendSV Handler. The sequence is  :

1️⃣Save the context of current task
2️⃣Select next task
3️⃣Retrieve the context of next task
4️⃣Resume next task execution
 Why i use pendsv to do context switch ?

The context switch could be performed by the SysTick_Handler with a SysTick timer configured to fire interrupts periodically:

Context switch performed by SysTick

This approach would however not work with other interrupts (peripheral interrupt for example). The SysTick_Handler would stack registers affected by the peripheral IRQ handler and unstack task’s registers, resulting in undefined behavior of both tasks and peripheral interrupt handler:

Context switch performed by SysTick - problem with IRQ
![image](https://github.com/hazemzain/simple_priodic_scheduler/assets/71934919/44044c75-09cd-45e5-a323-37ef7831f009)
 The solution is simple - the SysTick_Handler with the highest priority only selects the next task to be run and triggers PendSV interrupt. The PendSV_Handler with the lowest priority performs the actual context switch once all interrupt requests with higher priority have been handled:
 
 ![image](https://github.com/hazemzain/simple_priodic_scheduler/assets/71934919/18d1cd65-0b9d-4898-827e-d92097178cd9)
