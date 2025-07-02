  .align 2
trap_vector:
  # Save registers.
  addi    sp, sp, -CONTEXT_SIZE
  sxsp    ra, 0
  sxsp    a0, 1
  sxsp    a1, 2
  sxsp    a2, 3
  sxsp    a3, 4
  sxsp    a4, 5
  sxsp    a5, 6
  sxsp    a6, 7
  sxsp    a7, 8
  sxsp    t0, 9
  sxsp    t1, 10
  sxsp    t2, 11
  sxsp    t3, 12
  sxsp    t4, 13
  sxsp    t5, 14
  sxsp    t6, 15

  # Invoke the handler.
  jal     trap_handler
  

  .option push
  .option norelax
  .option arch, +zicsr 
  csrr t0, mepc
  addi t0, t0, 4
  csrw mepc, t0
  .option pop
  # Restore registers.
  lxsp    ra, 0
  lxsp    a0, 1
  lxsp    a1, 2
  lxsp    a2, 3
  lxsp    a3, 4
  lxsp    a4, 5
  lxsp    a5, 6
  lxsp    a6, 7
  lxsp    a7, 8
  lxsp    t0, 9
  lxsp    t1, 10
  lxsp    t2, 11
  lxsp    t3, 12
  lxsp    t4, 13
  lxsp    t5, 14
  lxsp    t6, 15
  addi sp, sp, CONTEXT_SIZE
  mret

  .align 2
trap_handler:
  li t0, 93
  beq t0, a7, sys_exit
  ret

#write exit for qemu
sys_exit:
  li     t0,0x100000
  beq    a0, zero, .exit_0
  li     t1,0x3333
  j      .exit_store
  .exit_0:
  li     t1,0x5555
  .exit_store:
  sw     t1,0(t0)

