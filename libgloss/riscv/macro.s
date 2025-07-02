# See LICENSE for license details.

.equ REGBYTES, 16
.equ SAVE_REGS,    16 
.equ CONTEXT_SIZE, (SAVE_REGS * REGBYTES) 

#if __riscv_xlen == 128
.macro lx a, b
lq \a, \b
.endm

.macro sx a, b
sq \a, \b
.endm

.macro lxsp a, b
lq \a, ((\b)*REGBYTES)(sp)
.endm

.macro sxsp a, b
sq \a, ((\b)*REGBYTES)(sp)
.endm

.macro .ptr a
.octa \a
.endm
#elif __riscv_xlen == 64
.macro lx a, b
ld \a, \b
.endm

.macro sx a, b
sd \a, \b
.endm

.macro lxsp a, b
ld \a, ((\b)*REGBYTES)(sp)
.endm

.macro sxsp a, b
sd \a, ((\b)*REGBYTES)(sp)
.endm

.macro .ptr a
.octa \a
.endm
#else
.macro lx a, b
lw \a, \b
.endm

.macro sx a, b
sw \a, \b
.endm

.macro lxsp a, b
lw \a, ((\b)*REGBYTES)(sp)
.endm

.macro sxsp a, b
sw \a, ((\b)*REGBYTES)(sp)
.endm

.macro .ptr a
.octa \a
.endm

#endif
