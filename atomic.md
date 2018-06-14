```asm  
static inline int fetch_and_add(int* variable, int value)
  {
      __asm__ volatile("lock; xaddl %0, %1"
        : "+r" (value), "+m" (*variable) // input+output
        : // No input-only
        : "memory"
      );
      return value;
  }
  
  
  // ++atomic
        lock xaddl      %eax, (%rdx)
        addl    $1, %eax
        
 // atomic++
        lock xaddl      %edx, (%rax)
        movl    %edx, %eax
  ```
