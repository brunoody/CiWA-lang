(module
  (import "console" "log" (func $println (param i32)))
  
  (memory 1 1024)
  (data (i32.const 0) "")
  (data (i32.const 1) "code")
  (data (i32.const 2) "...reached")
  (data (i32.const 3) "...not reached")
  (data (i32.const 4) "end of code...")

  (func (export "main")
     (local $i i32)
     (local.set $i (i32.const 0))
     
     i32.const 0
     call $println

     i32.const 1
     call $println
     
     local.get $i
     i32.const 0
     i32.eq
     (if
        (then
           i32.const 2
           call $println
        )
     )
    
     local.get $i
     i32.const 0
     i32.gt_u
     (if
        (then
           i32.const 3
           call $println
        )
        ;; (else )
     )
    
     i32.const 0
     call $println
    
     i32.const 4
     call $println
  )

  (start 1) ;; run the first function automatically
)