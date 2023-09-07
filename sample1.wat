(module
  (import "env" "log" (func $println (param i32 i32)))
  (import "env" "memory" (memory 1))

  (data (offset (i32.const 0)) "")
  (data (offset (i32.const 1)) "code")
  (data (offset (i32.const 2)) "...reached")
  (data (offset (i32.const 3)) "...not reached")
  (data (offset (i32.const 4)) "end of code...")

  (func (export "main")
     (local $i i32)
     (local.set $i (i32.const 0))
     
     i32.const 0
     i32.const 0
     call $println

     i32.const 1
     i32.const 4
     call $println
     
     local.get $i
     i32.const 0
     i32.eq
     (if
        (then
           i32.const 2
           i32.const 0
           call $println
        )
     )
    
     local.get $i
     i32.const 0
     i32.gt_u
     (if
        (then
           i32.const 3
           i32.const 14
           call $println
        )
        ;; (else )
     )
    
     i32.const 0
     i32.const 0
     call $println
    
     i32.const 4
     i32.const 14
     call $println
  )

  (start 1) ;; run the first function automatically
)