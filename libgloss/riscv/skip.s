skip:
  # setup default trap vector
  la      t0, trap_vector
  .option push
  .option norelax
  .option arch, +zicsr
  csrw    mtvec, t0
  .option pop
  la sp, __heap_end
  j _start
