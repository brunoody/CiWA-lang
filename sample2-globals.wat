;; Example of WAT possibly generated relating to "sample1.cw"
;; 
(module
    (import "env" "log" (func $println (param i32 i32)))
  	(import "env" "memory" (memory 1))

    (data (offset (i32.const 0)) "a is 1")        ;; storing "a is 1"
    (global $stroff_0x00000000 i32 (i32.const 0)) ;; offset of "a is 1" = 0
    (global $strlen_0x00000000 i32 (i32.const 6)) ;; length of "a is 1" = 6

    (data (offset (i32.const 1)) "b is 1")        ;; storing "b is 1"
    (global $stroff_0x00000001 i32 (i32.const 1)) ;; offset of "b is 1" = 1
    (global $strlen_0x00000001 i32 (i32.const 6)) ;; length of "a is 1" = 6

    (global $var_0x00000000 (mut i32) (i32.const 0)) ;; int32 i 0
    (global $var_0x00000001 (mut i32) (i32.const 0)) ;; int32 a 0
    (global $var_0x00000002 (mut i32) (i32.const 0)) ;; int32 b 0

    (func (export "main")
      	global.get $var_0x00000000
        i32.const 0
        i32.eq ;; if i == 0
        (if (then 
          		i32.const 1
          		global.set $var_0x00000001
      	))
        global.get $var_0x00000000
        i32.const 0
        i32.gt_s ;; if i > 0
        (if (then 
          		i32.const 1
          		global.set $var_0x00000002
      	))
        global.get $var_0x00000001
        i32.const 1
        i32.eq ;; if a == 1
        (if (then 
                global.get $stroff_0x00000000 ;; offset for "a is 1"
                global.get $strlen_0x00000000
                call $println
      	))
        global.get $var_0x00000002
        i32.const 1
        i32.eq ;; if b == 1
        (if (then 
                global.get $stroff_0x00000001 ;; offset for "b is 1"
                global.get $strlen_0x00000001
                call $println
      	))
    )

    (start 1) ;; run the first function automatically
)