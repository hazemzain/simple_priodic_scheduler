			AREA |.text|,CODE,READONLY,ALIGN=2
			THUMB
			PRESERVE8
			EXTERN currentPt
		    EXPORT PendSV_Handler
			EXPORT osSchedularLaunch
			IMPORT osSchedulerPeriodicRR

PendSV_Handler   ;save r0,r1,r2,r3,r12,lr,pc,psr  
	CPSID	  I
	PUSH 	  {R4-R11}
	LDR 	  R0,=currentPt
	LDR		  R1,[R0]
	
	STR 	  SP,[R1]
	;LDR 	  R1,[R1,#4]   ;select the the next thread hich will be excuted(They select the next the next member, these two lines.Selects the next threat put put in these two lines together, we are able to sort of switch to the next)
	;STR 	  R1,[R0]
	push      {R0,LR}
	;Branch to subroutin which e ill write with c
	BL        osSchedulerPeriodicRR
	pop       {R0,LR}
	; I'm going to set the new thread 
	LDR      R1,[R0]   ;R1=Currentptr ->new thread
	LDR 	  SP,[R1]
	
	POP		  {R4-R11}
	CPSIE	  I
	BX		  LR
	

osSchedularLaunch
	LDR		R0,=currentPt
	LDR		R2,[R0]
	
	LDR		SP,[R2]
	
	POP		{R4-R11}
	POP		{R0-R3}
	POP 	{R12}
	ADD		SP,SP,#4
	POP		{LR}
	ADD		SP,SP,#4
	CPSIE    I
	BX		 LR
	ALIGN
	END