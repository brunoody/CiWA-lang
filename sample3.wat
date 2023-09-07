(module
    (import "console" "log" (func $println (param i32 i32)))

    (memory 1 1024)
    (data (i32.const 0) "Hello and welcome to my third test!")
    (data (i32.const 1) "value = ")

    (func (export "main") 
        (local $vstring0 i32) (local $vstring1 i32)
        (local $i i32) (local $str i32) (local $j i32)

        ;; first, initialize all v-string pointers - we must know the position and/or range of each one
        i32.const 0
        local.set $vstring0
        ;; i.e.: $vstring1 starts at position 1 and ends at position 100, because it's in a loop and we know it
        i32.const 1
        local.set $vstring1

        ;; variable 'int i'
        i32.const 0
        local.set $i

        ;; 'var str' attribution
        i32.const 0 ;; setting with the data memory position (known from compilation)
        local.set $str

        ;; println statement
        local.get $vstring0 ;; get the position set (known from compilation)
        i32.const 35 ;; length of the text (known from compilation)
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

        ;; for loop
        i32.const 1
        local.set $j
        (block $for0
            (loop
                ;; test if $j is greater than 100 and breaks the loop if true
                local.get $j
                i32.const 100
                i32.gt_s
                br_if 1
                
                ;; println statement (constant part)
                local.get $vstring1
                i32.const 8 ;; length of the text (known from compilation)
                call $println
                ;; println statement (variable part from $j)
                local.get $j
                i32.const 1
                call $println

                ;; makes the final increment of the For-loop
                local.get $j
                i32.const 1
                i32.add
                local.set $j

                br 0 ;; send back to loop-start
            )
        )
    )
)