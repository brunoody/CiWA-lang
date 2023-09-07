(module
    (import "env" "log" (func $println (param i32 i32)))
  	(import "env" "memory" (memory 1))
    
    (data (i32.const 0) "Hello and welcome to my third test!")
    (global $stroff_0x00000000 i32 (i32.const 0)) ;; offset 0
    (global $strlen_0x00000000 i32 (i32.const 35)) ;; length 35

    ;; --- $vstring0 section ---
    (global $stroff_0x00000001 i32 (i32.const 35)) ;; offset 35

    (data (i32.const 35) "i = 99")
    (global $vstringlen_0x00000000 i32 (i32.const 6)) ;; length 6
    ;; -------------------------

    ;; --- $vstring1 section ---
    ;; one data position for each v-string interpolated!!!
    (global $vstringlen_0x00000001 i32 (i32.const 10)) ;; max length is 10 for $vstring1

    (data (i32.const 42) "value = 1 ")
    (global $stroff_0x00000002 i32 (i32.const 42)) ;; offset 42

    (data (i32.const 52) "value = 2 ")
    (global $stroff_0x00000003 i32 (i32.const 52))

    (data (i32.const 62) "value = 3 ")
    (global $stroff_0x00000004 i32 (i32.const 62))

    (data (i32.const 72) "value = 4 ")
    (global $stroff_0x00000005 i32 (i32.const 72))

    (data (i32.const 82) "value = 5 ")
    (global $stroff_0x00000006 i32 (i32.const 82))

    (data (i32.const 92) "value = 6 ")
    (global $stroff_0x00000007 i32 (i32.const 92))

    (data (i32.const 102) "value = 7 ")
    (global $stroff_0x00000008 i32 (i32.const 102))

    (data (i32.const 112) "value = 8 ")
    (global $stroff_0x00000009 i32 (i32.const 112))

    (data (i32.const 122) "value = 9 ")
    (global $stroff_0x00000010 i32 (i32.const 122))

    (data (i32.const 132) "value = 10")
    (global $stroff_0x00000011 i32 (i32.const 132)) ;; offset 132 (9*10 + 42)
    ;; -------------------------

    (func (export "main")
        (local $vstringoff_0x00000000 i32)
        (local $vstringoff_0x00000001 i32)
        (local $i i32) (local $str i32) (local $j i32)

        ;; first, initialize all v-string pointers - we must know the position and/or range of each one
        global.get $stroff_0x00000001
        local.set $vstringoff_0x00000000
        ;; i.e.: $vstring1 starts at position 1 and ends at position 100, because it's in a loop and we know it
        global.get $stroff_0x00000002
        local.set $vstringoff_0x00000001

        ;; variable 'int i' init
        i32.const 0
        local.set $i

        ;; 'var str' attribution
        global.get $stroff_0x00000000 ;; setting with the data memory position (known from compilation)
        local.set $str

        ;; println statement
        global.get $stroff_0x00000000
        global.get $strlen_0x00000000
        call $println

        ;; while loop - the translation below from the While-loop in the example 'sample3.cw' is not good in WASM!!!
        (block $while0
            (loop
                ;; test if $i is greater than 99 and breaks the loop if true
                local.get $i
                i32.const 99
                i32.gt_s
                br_if 1
                
                ;; i++ statement
                i32.const 1
                local.get $i
                i32.add
                local.set $i

                br 0 ;; send back to loop-start
            )
        )

        ;; println "i = {i}"v
        local.get $vstringoff_0x00000000
        global.get $vstringlen_0x00000000
        call $println

        ;; for loop
        i32.const 1
        local.set $j
        (block $for0
            (loop
                ;; test if $j is greater than 10 and breaks the loop if true
                local.get $j
                i32.const 10
                i32.gt_s
                br_if 1
                
                ;; println "value = {j}"v statement
                local.get $vstringoff_0x00000001
                global.get $vstringlen_0x00000001
                call $println
                ;; increment the iteratable v-string position into memory
                global.get $vstringlen_0x00000001
                local.get $vstringoff_0x00000001
                i32.add
                local.set $vstringoff_0x00000001

                ;; makes the final increment of the For-loop
                local.get $j
                i32.const 1
                i32.add
                local.set $j

                br 0 ;; send back to loop-start
            )
        )
    )

    (start 1) ;; run the first function automaticallys
)