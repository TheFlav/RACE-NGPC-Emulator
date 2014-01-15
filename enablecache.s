@ asmfunc.s
@ void CPU_alignment_cache(void);
	.ARM
	.GLOBAL EnableCache

EnableCache:
	stmfd	r13!,{r5,r6}

	mrc		p15, 0, r5, c1, c0, 0
	bic		r5,r5,#2
	ldr		r6,=0x1004
	orr		r5,r5,r6
	mcr		p15, 0, r5, c1, c0, 0

	ldmfd	r13!,{r5,r6}
	bx		lr

	.end
